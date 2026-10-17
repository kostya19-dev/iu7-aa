from __future__ import annotations

import argparse
import csv
import re
from pathlib import Path
from typing import Callable, List, Optional, Tuple

from PyPDF2 import PdfReader

Line = Tuple[str, int, int]  # (text, page_no_1based, line_no_1based)
Coord = Tuple[int, int]
Match = Tuple[str, Coord]

DEFAULT_SET = [
    *(f"_{i:02d}.pdf" for i in range(1, 11)),
    "main (2).pdf",
    "ВКР Селез.pdf",
]


# ----------------------------
# ALGORITHM (DeepSeek-style core)
# ----------------------------

def rx_spaced(word: str) -> str:
    return r"\s*".join(re.escape(ch) for ch in word)


RX_TOC = re.compile(r"^\s*(СОДЕРЖАНИЕ|ОГЛАВЛЕНИЕ)\s*$", re.I)
RX_TOC_ENTRY = re.compile(r"^(?=.*[А-Яа-яA-Za-z])(?=.*\d)\s*.+\d+\s*$")

INTRO_OK = rf"{rx_spaced('ВВЕДЕНИЕ')}\b"
CONCL_OK = rf"{rx_spaced('ЗАКЛЮЧЕНИЕ')}\b"
REFS_OK = rf"{rx_spaced('СПИСОК')}\s+{rx_spaced('ИСПОЛЬЗОВАННЫХ')}\s+{rx_spaced('ИСТОЧНИКОВ')}\b"

RE_INTRO_TOC = re.compile(rf"^\s*{INTRO_OK}", re.I)
RE_CONCL_TOC = re.compile(rf"^\s*{CONCL_OK}", re.I)
RE_REFS_TOC = re.compile(rf"^\s*{REFS_OK}", re.I)

def mk_heading(ok_pat: str) -> re.Pattern:
    return re.compile(rf"^\s*(?:\d+(?:\.\d+)*)?\s*{ok_pat}\s*$", re.I)

RE_INTRO_H = mk_heading(INTRO_OK)
RE_CONCL_H = mk_heading(CONCL_OK)
RE_REFS_H = mk_heading(REFS_OK)

RE_BAD_ALL = [
    re.compile(r"^\s*(?:\d+(?:\.\d+)*)?\s*(ВСТУПЛЕНИЕ|ПРЕДИСЛОВИЕ)\s*$", re.I),
    re.compile(r"^\s*(?:\d+(?:\.\d+)*)?\s*(ВЫВОДЫ|ИТОГИ|ЗАКЛЮЧЕНИЯ)\s*$", re.I),
    re.compile(r"^\s*(?:\d+(?:\.\d+)*)?\s*(СПИСОК\s+ЛИТЕРАТУРЫ|ЛИТЕРАТУРА|БИБЛИОГРАФИЯ|БИБЛИОГРАФИЧЕСКИЙ\s+СПИСОК|СПИСОК\s+ИСТОЧНИКОВ)\s*$", re.I),
]


def extract_lines(pdf: str | Path) -> List[Line]:
    r = PdfReader(str(pdf))
    out: List[Line] = []
    for p, page in enumerate(r.pages, 1):
        text = page.extract_text() or ""
        for i, s in enumerate(text.splitlines(), 1):
            s = s.strip()
            if s:
                out.append((s, p, i))
    return out


def find_toc_slice(lines: List[Line], cap: int = 500) -> Tuple[int, int]:
    start = -1
    for idx, (t, _, _) in enumerate(lines):
        if RX_TOC.match(t):
            start = idx
            break
    if start == -1:
        return -1, -1

    end = start + 1
    miss = 0
    for j in range(start + 1, min(len(lines), start + 1 + cap)):
        if RX_TOC_ENTRY.match(lines[j][0]):
            end = j + 1
            miss = 0
        else:
            miss += 1
            if miss >= 5:
                break
    return start, end


def first(where: List[Line], pred: Callable[[str], bool]) -> Optional[Line]:
    for t, p, i in where:
        if pred(t):
            return (t, p, i)
    return None


def check_variant_structure(pdf: str | Path) -> Tuple[bool, List[Match]]:
    ls = extract_lines(pdf)
    a, b = find_toc_slice(ls)
    if a == -1:
        return False, []

    toc = ls[a:b]
    body = ls[:a] + ls[b:]

    intro = first(toc, lambda s: RE_INTRO_TOC.search(s) is not None)
    concl = first(toc, lambda s: RE_CONCL_TOC.search(s) is not None)
    refs = first(toc, lambda s: RE_REFS_TOC.search(s) is not None)
    if not (intro and concl and refs):
        return False, []

    # В тексте корректных заголовков быть не должно
    if first(body, lambda s: RE_INTRO_H.match(s) or RE_CONCL_H.match(s) or RE_REFS_H.match(s)):
        return False, []

    # Ошибочных наименований быть не должно
    if first(ls, lambda s: any(rx.match(s) for rx in RE_BAD_ALL)):
        return False, []

    return True, [(intro[0], (intro[1], intro[2])),
                 (concl[0], (concl[1], concl[2])),
                 (refs[0], (refs[1], refs[2]))]


# ----------------------------
# OUTPUT + CLI (CSV)
# ----------------------------

def pick_inputs(path: Path) -> List[Path]:
    if path.is_file():
        return [path]
    picked = []
    for name in DEFAULT_SET:
        p = path / name
        if p.exists() and p.is_file():
            picked.append(p)
    return picked if picked else sorted(path.glob("*.pdf"))


def status_text(error_found: bool) -> str:
    return "найдена ошибка" if error_found else "ошибка не найдена"


def first_coord(ok: bool, matches: List[Match]) -> str:
    if (not ok) or (not matches):
        return "-"
    p, l = matches[0][1]
    return f"({p},{l})"


def run_one(p: Path) -> Tuple[str, str, str]:
    ok, matches = check_variant_structure(p)
    error_found = not ok
    return (p.name, status_text(error_found), first_coord(ok, matches))


def write_csv(rows: List[Tuple[str, str, str]], out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8", newline="") as f:
        w = csv.writer(f)
        w.writerow(["file", "status", "first_coord"])
        w.writerows(rows)


def main() -> None:
    ap = argparse.ArgumentParser(description="DeepSeek-версия: проверка варианта + CSV.")
    ap.add_argument("path", type=str, help="PDF или каталог")
    ap.add_argument("--out", type=str, default="results_deepseek.csv", help="CSV output")
    args = ap.parse_args()

    inputs = pick_inputs(Path(args.path))
    rows = [run_one(p) for p in inputs]
    write_csv(rows, Path(args.out))


if __name__ == "__main__":
    main()
