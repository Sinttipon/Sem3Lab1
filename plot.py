import csv
import math

import matplotlib.pyplot as plt

SERIES = [
    ("insert_random", "вставка (случайные)", "#2a78d6", "o"),
    ("insert_sorted", "вставка (по порядку)", "#eb6834", "s"),
    ("search", "поиск", "#1baf7a", "^"),
    ("remove", "удаление", "#eda100", "D"),
]

with open("results.csv", encoding="utf-8") as f:
    rows = list(csv.DictReader(f))
n = [int(r["n"]) for r in rows]

fig, (total_ax, per_op_ax) = plt.subplots(1, 2, figsize=(13, 5.5))

for key, label, color, marker in SERIES:
    ms = [float(r[key]) for r in rows]
    per_op_us = [t * 1000 / count for t, count in zip(ms, n)]
    for ax, ys in ((total_ax, ms), (per_op_ax, per_op_us)):
        ax.plot(n, ys, color=color, marker=marker, markersize=7, linewidth=2, label=label)

first = float(rows[0]["insert_random"])
scale = first / (n[0] * math.log2(n[0]))
total_ax.plot(n, [scale * k * math.log2(k) for k in n], color="#999999",
              linestyle="--", linewidth=1.5, label="n·log n (для сравнения)")

total_ax.set_xscale("log")
total_ax.set_yscale("log")
total_ax.set_title("Общее время на n операций")
total_ax.set_ylabel("время, мс (лог. шкала)")

per_op_ax.set_xscale("log")
per_op_ax.set_title("Время одной операции")
per_op_ax.set_ylabel("время, мкс")

for ax in (total_ax, per_op_ax):
    ax.set_xlabel("количество элементов n (лог. шкала)")
    ax.grid(True, which="major", color="#e5e5e5", linewidth=0.8)
    ax.spines[["top", "right"]].set_visible(False)
    ax.legend(frameon=False, fontsize=9, loc="upper left")

fig.suptitle("AVL-дерево: зависимость времени от количества элементов")
fig.tight_layout()
fig.savefig("plot.png", dpi=150)
print("Saved to plot.png")
