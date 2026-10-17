from __future__ import annotations

import argparse
import csv
import re
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional, Tuple

from PyPDF2 import PdfReader

Coord = Tuple[int, int]
Match = Tuple[str, Coord]

DEFAULT_SET = [
    *(f"_{i:02d}.pdf" for i in range(1, 11)),
    "main (2).pdf",
    "ВКР Селез.pdf",
]


@dataclass
class Line:
    t: str
    p: int
    i: int


class VariantChecker:
    def __init__(self) -> None:
        self.rx_toc = re.compile(r"^\s*(СОДЕРЖАНИЕ|ОГЛАВЛЕНИЕ)\s*$", re.I)
        self.rx_toc_entry = re.compile(r"^(?=.*[А-Яа-яA-Za-z])(?=.*\d)\s*.+\d+\s*$")

        def spaced(w: str) -> str:
            return r"\s*".join(re.escape(ch) for ch in w)

        self.rx_intro_ok = re.compile(rf"^\s*{spaced('ВВЕДЕНИЕ')}\b", re.I)
        self.rx_concl_ok = re.compile(rf"^\s*{spaced('ЗАКЛЮЧЕНИЕ')}\b", re.I)
        self.rx_refs_ok = re.compile(
            rf"^\s*{spaced('СПИСОК')}\s+{spaced('ИСПОЛЬЗОВАННЫХ')}\s+{spaced('ИСТОЧНИКОВ')}\b", re.I
        )

        self.rx_intro_h = self._mk_heading(self.rx_intro_ok.pattern)
        self.rx_concl_h = self._mk_heading(self.rx_concl_ok.pattern)
        self.rx_refs_h = self._mk_heading(self.rx_refs_ok.pattern)

        self.rx_bad = [
            re.compile(r"^\s*(?:\d+(?:\.\d+)*)?\s*(ВСТУПЛЕНИЕ|ПРЕДИСЛОВИЕ)\s*$", re.I),
            re.compile(r"^\s*(?:\d+(?:\.\d+)*)?\s*(ВЫВОДЫ|ИТОГИ|ЗАКЛЮЧЕНИЯ)\s*$", re.I),
            re.compile(
                r"^\s*(?:\d+(?:\.\d+)*)?\s*(СПИСОК\s+ЛИТЕРАТУРЫ|ЛИТЕРАТУРА|БИБЛИОГРАФИЯ|БИБЛИОГРАФИЧЕСКИЙ\s+СПИСОК|СПИСОК\s+ИСТОЧНИКОВ)\s*$",
                re.I,
            ),
        ]

    def _mk_heading(self, ok_pat: str) -> re.Pattern:
        return re.compile(rf"^\s*(?:\d+(?:\.\d+)*)?\s*({ok_pat})\s*$", re.I)

    def extract(self, pdf: Path) -> List[Line]:
        r = PdfReader(str(pdf))
        out: List[Line] = []
        for p, page in enumerate(r.pages, 1):
            txt = page.extract_text() or ""
            for i, s in enumerate(txt.splitlines(), 1):
                s = s.strip()
                if s:
                    out.append(Line(s, p, i))
        return out

    def toc_region(self, ls: List[Line], cap: int = 500) -> Tuple[int, int]:
        start = -1
        for idx, ln in enumerate(ls):
            if self.rx_toc.match(ln.t):
                start = idx
                break
        if start == -1:
            return -1, -1

        end = start + 1
        miss = 0
        for j in range(start + 1, min(len(ls), start + 1 + cap)):
            if self.rx_toc_entry.match(ls[j].t):
                end = j + 1
                miss = 0
            else:
                miss += 1
                if miss >= 5:
                    break
        return start, end

    def first(self, where: List[Line], rx: re.Pattern) -> Optional[Line]:
        for ln in where:
            if rx.search(ln.t):
                return ln
        return None

    def check_variant_structure(self, pdf: Path) -> Tuple[bool, List[Match]]:
        ls = self.extract(pdf)
        a, b = self.toc_region(ls)
        if a == -1:
            return False, []

        toc = ls[a:b]
        body = ls[:a] + ls[b:]

        intro = self.first(toc, self.rx_intro_ok)
        concl = self.first(toc, self.rx_concl_ok)
        refs = self.first(toc, self.rx_refs_ok)
        if not (intro and concl and refs):
            return False, []

        # В тексте корректных заголовков быть не должно
        if self.first(body, self.rx_intro_h) or self.first(body, self.rx_concl_h) or self.first(body, self.rx_refs_h):
            return False, []

        # Ошибочных наименований быть не должно
        for rx in self.rx_bad:
            if self.first(ls, rx):
                return False, []

        return True, [(intro.t, (intro.p, intro.i)),
                     (concl.t, (concl.p, concl.i)),
                     (refs.t, (refs.p, refs.i))]


def pick_inputs(path: Path) -> List[Path]:
    if path.is_file():
        return [path]
    picked: List[Path] = []
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


def write_csv(rows: List[Tuple[str, str, str]], out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8", newline="") as f:
        w = csv.writer(f)
        w.writerow(["file", "status", "first_coord"])
        w.writerows(rows)


def main() -> None:
    ap = argparse.ArgumentParser(description="Qwen-версия: проверка варианта + CSV.")
    ap.add_argument("path", type=str, help="PDF или каталог")
    ap.add_argument("--out", type=str, default="results_qwen.csv", help="CSV output")
    args = ap.parse_args()

    checker = VariantChecker()
    inputs = pick_inputs(Path(args.path))

    rows: List[Tuple[str, str, str]] = []
    for p in inputs:
        ok, matches = checker.check_variant_structure(p)
        error_found = not ok
        rows.append((p.name, status_text(error_found), first_coord(ok, matches)))

    write_csv(rows, Path(args.out))


if __name__ == "__main__":
    main()
