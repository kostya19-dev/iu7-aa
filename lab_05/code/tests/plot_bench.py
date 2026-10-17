import csv
import os
import sys

import matplotlib.pyplot as plt


def read_one_csv(path):
    rows = []
    with open(path, "r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        for r in reader:
            mode = (r.get("mode") or "").strip()
            n = int((r.get("N") or "0").strip())
            t = float((r.get("time_ms") or "0").strip())
            rows.append((mode, n, t))
    rows.sort(key=lambda x: x[1])
    return rows


def series_label(path, rows):
    base = os.path.splitext(os.path.basename(path))[0]
    if rows:
        modes = sorted({m for m, _, _ in rows if m})
        if len(modes) == 1:
            return modes[0]
    return base


def plot_many(csv_paths, out_png):
    plt.rcParams.update({
        "font.size": 12,
        "axes.titlesize": 18,
        "axes.labelsize": 13,
        "legend.fontsize": 11,
        "lines.linewidth": 2.6,
        "lines.markersize": 5.5,
    })

    fig, ax = plt.subplots(figsize=(12.8, 7.2), dpi=170)

    linestyles = ["-", "--", "-.", ":"]
    markers = ["o", "s", "D", "^", "v", "P", "X", "<", ">"]

    xticks = None

    for i, p in enumerate(csv_paths):
        rows = read_one_csv(p)
        if not rows:
            continue

        xs = [n for _, n, _ in rows]
        ys = [t for _, _, t in rows]
        label = series_label(p, rows)

        if xticks is None:
            xticks = xs

        ls = linestyles[i % len(linestyles)]
        mk = markers[i % len(markers)]

        if len(xs) == 1:
            ax.scatter(xs, ys, label=label)
        else:
            ax.plot(xs, ys, linestyle=ls, marker=mk, label=label)

    ax.set_title("Сравнение времени выполнения")
    ax.set_xlabel("N (число заявок)")
    ax.set_ylabel("Время (мс)")

    if xticks:
        ax.set_xticks(xticks)

    ax.grid(True, which="major", linestyle="-", alpha=0.35)
    ax.grid(True, which="minor", linestyle="--", alpha=0.18)
    ax.minorticks_on()

    ax.legend(loc="upper left", frameon=True)

    fig.tight_layout()
    fig.savefig(out_png)
    plt.close(fig)


def main():
    if len(sys.argv) < 2:
        print("Запуск:")
        print("  python3 plot_multi.py bench_seq.csv bench_pipe3.csv bench_pipe4.csv bench_seq_plus.csv")
        return 1

    paths = sys.argv[1:]
    for p in paths:
        if not os.path.exists(p):
            print("Не найден файл:", p)
            return 1

    plot_many(paths, "bench_compare.png")
    print("Готово: bench_compare.png")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
