from __future__ import annotations

import argparse
import csv
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Optional, Tuple

from PyPDF2 import PdfReader

# ----------------------------
# ALGORITHM (core logic)
# ----------------------------

Coord = Tuple[int, int]  # (page_no_1based, line_no_1based)
Match = Tuple[str, Coord]


@dataclass(frozen=True)
class Line:
    text: str
    page_no: int
    line_no: int


def _spaced_letters(word: str) -> str:
    return r"\s*".join(re.escape(ch) for ch in word)


RE_TOC_HEADING = re.compile(r"^\s*(СОДЕРЖАНИЕ|ОГЛАВЛЕНИЕ)\s*$", re.IGNORECASE)
RE_TOC_ENTRY_SHAPE = re.compile(r"^(?=.*[А-Яа-яA-Za-z])(?=.*\d)\s*.+\d+\s*$")

RE_INTRO_OK = re.compile(rf"^\s*{_spaced_letters('ВВЕДЕНИЕ')}\b", re.IGNORECASE)
RE_CONCL_OK = re.compile(rf"^\s*{_spaced_letters('ЗАКЛЮЧЕНИЕ')}\b", re.IGNORECASE)
RE_REFS_OK = re.compile(
    rf"^\s*{_spaced_letters('СПИСОК')}\s+{_spaced_letters('ИСПОЛЬЗОВАННЫХ')}\s+{_spaced_letters('ИСТОЧНИКОВ')}\b",
    re.IGNORECASE,
)


def _as_heading_line(re_ok: re.Pattern) -> re.Pattern:
    return re.compile(
        rf"^\s*(?:\d+(?:\.\d+)*)?\s*({re_ok.pattern})\s*$",
        re.IGNORECASE,
    )


RE_INTRO_HEADING = _as_heading_line(RE_INTRO_OK)
RE_CONCL_HEADING = _as_heading_line(RE_CONCL_OK)
RE_REFS_HEADING = _as_heading_line(RE_REFS_OK)

RE_BAD_INTRO = re.compile(
    r"^\s*(?:\d+(?:\.\d+)*)?\s*(ВСТУПЛЕНИЕ|ПРЕДИСЛОВИЕ)\s*$",
    re.IGNORECASE,
)
RE_BAD_CONCL = re.compile(
    r"^\s*(?:\d+(?:\.\d+)*)?\s*(ВЫВОДЫ|ИТОГИ|ЗАКЛЮЧЕНИЯ)\s*$",
    re.IGNORECASE,
)
RE_BAD_REFS = re.compile(
    r"^\s*(?:\d+(?:\.\d+)*)?\s*("
    r"СПИСОК\s+ЛИТЕРАТУРЫ|ЛИТЕРАТУРА|БИБЛИОГРАФИЯ|"
    r"БИБЛИОГРАФИЧЕСКИЙ\s+СПИСОК|СПИСОК\s+ИСТОЧНИКОВ"
    r")\s*$",
    re.IGNORECASE,
)


def extract_lines_from_pdf(pdf_path: str | Path) -> List[Line]:
    reader = PdfReader(str(pdf_path))
    lines: List[Line] = []
    for p_idx, page in enumerate(reader.pages, start=1):
        text = page.extract_text() or ""
        raw_lines = text.splitlines()
        for l_idx, s in enumerate(raw_lines, start=1):
            s_norm = s.strip()
            if s_norm:
                lines.append(Line(text=s_norm, page_no=p_idx, line_no=l_idx))
    return lines


def _find_toc_region(lines: List[Line], max_follow_lines: int = 500) -> Tuple[int, int]:
    toc_start = -1
    for i, ln in enumerate(lines):
        if RE_TOC_HEADING.match(ln.text):
            toc_start = i
            break
    if toc_start == -1:
        return -1, -1

    end = toc_start + 1
    non_toc_streak = 0

    for j in range(toc_start + 1, min(len(lines), toc_start + 1 + max_follow_lines)):
        t = lines[j].text

        if RE_TOC_ENTRY_SHAPE.match(t):
            end = j + 1
            non_toc_streak = 0
            continue

        # допускаем 1–2 "служебные" строки внутри оглавления
        if len(t) >= 20 and non_toc_streak <= 1:
            end = j + 1
            non_toc_streak += 1
            continue

        non_toc_streak += 1
        if non_toc_streak >= 5:
            break

    return toc_start, end


def check_variant_structure(pdf_path: str | Path) -> Tuple[bool, List[Match]]:
    """
    Вариант: проверка отсутствия структурных элементов ВВЕДЕНИЕ/ЗАКЛЮЧЕНИЕ/СПИСОК...
    в тексте (вне оглавления), их наличия в оглавлении и отсутствие ошибочных наименований.

    Возврат:
      (ok, matches), где:
        ok: bool
        matches: список кортежей (найденная строка, (страница, строка)) — только если ok=True
    """
    lines = extract_lines_from_pdf(pdf_path)
    toc_start, toc_end = _find_toc_region(lines)

    if toc_start == -1:
        return False, []

    toc_lines = lines[toc_start:toc_end]
    body_lines = lines[:toc_start] + lines[toc_end:]

    def _first_in(where: Iterable[Line], rx: re.Pattern) -> Optional[Line]:
        for ln in where:
            if rx.search(ln.text):
                return ln
        return None

    # 1) В оглавлении должны быть все 3 корректные строки
    intro_toc = _first_in(toc_lines, RE_INTRO_OK)
    concl_toc = _first_in(toc_lines, RE_CONCL_OK)
    refs_toc = _first_in(toc_lines, RE_REFS_OK)
    if not (intro_toc and concl_toc and refs_toc):
        return False, []

    # 2) В тексте (вне оглавления) НЕ должно быть корректных заголовков
    if _first_in(body_lines, RE_INTRO_HEADING) or _first_in(body_lines, RE_CONCL_HEADING) or _first_in(body_lines, RE_REFS_HEADING):
        return False, []

    # 3) Во всём документе НЕ должно быть ошибочных наименований
    if _first_in(lines, RE_BAD_INTRO) or _first_in(lines, RE_BAD_CONCL) or _first_in(lines, RE_BAD_REFS):
        return False, []

    matches: List[Match] = [
        (intro_toc.text, (intro_toc.page_no, intro_toc.line_no)),
        (concl_toc.text, (concl_toc.page_no, concl_toc.line_no)),
        (refs_toc.text, (refs_toc.page_no, refs_toc.line_no)),
    ]
    return True, matches


# ----------------------------
# OUTPUT + CLI (presentation)
# ----------------------------

DEFAULT_SET = [
    *(f"_{i:02d}.pdf" for i in range(1, 11)),
    "main (2).pdf",
    "ВКР Селез.pdf",
]


def _pick_inputs(path: Path) -> List[Path]:
    if path.is_file():
        return [path]

    # если это каталог: сначала пытаемся взять "обязательный" набор
    chosen: List[Path] = []
    for name in DEFAULT_SET:
        p = path / name
        if p.exists() and p.is_file():
            chosen.append(p)

    if chosen:
        return chosen

    # иначе — все pdf в каталоге
    return sorted(path.glob("*.pdf"))


def _first_coord_or_dash(ok: bool, matches: List[Match]) -> str:
    if not ok or not matches:
        return "-"
    (s, (p, l)) = matches[0]
    return f"({p},{l})"


def _status_text(error_found: bool) -> str:
    # пользователь просил дословно
    return "найдена ошибка" if error_found else "ошибка не найдена"


def run_one(pdf_path: Path) -> Tuple[str, str, str]:
    ok, matches = check_variant_structure(pdf_path)
    # по смыслу: ok=True => ошибок НЕТ (условия варианта выполнены)
    error_found = not ok
    return (pdf_path.name, _status_text(error_found), _first_coord_or_dash(ok, matches))


def write_csv(rows: List[Tuple[str, str, str]], out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8", newline="") as f:
        w = csv.writer(f)
        w.writerow(["file", "status", "first_coord"])
        w.writerows(rows)


def main() -> None:
    ap = argparse.ArgumentParser(
        description=(
            "Вариант: отсутствие ВВЕДЕНИЕ/ЗАКЛЮЧЕНИЕ/СПИСОК ИСПОЛЬЗОВАННЫХ ИСТОЧНИКОВ в тексте, "
            "их наличие в оглавлении и отсутствие ошибочных наименований. Экспорт в CSV."
        )
    )
    ap.add_argument("path", type=str, help="Путь к PDF-файлу или каталогу")
    ap.add_argument("--out", type=str, default="results_chatgpt.csv", help="Путь к CSV (по умолчанию results_chatgpt.csv)")
    args = ap.parse_args()

    inputs = _pick_inputs(Path(args.path))
    rows = [run_one(p) for p in inputs]
    write_csv(rows, Path(args.out))


if __name__ == "__main__":
    main()
