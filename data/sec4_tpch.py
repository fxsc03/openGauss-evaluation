import matplotlib.pyplot as plt

# ======================
# Common X axis (舍弃 4 DOP)
# ======================
dop = [8, 16, 32, 64, 96, 128]
queries = ["Q6", "Q3", "Q12", "Q5"]


# ======================
# Execution time
# b = baseline, o = optimized
# ======================
exec_time_b = {
    "Q3":  [22297, 11297, 5770, 5140, 5262, 6676],
    "Q5":  [11034, 5517, 3848, 3631, 5623, 6211],
    "Q12": [15054, 8027,6628, 6281, 6099, 9081],
    "Q6":  [2549, 1206, 688, 527, 452, 491],
}

exec_time_o = {
    "Q3":  [22145, 11088, 5921, 4797, 4844, 4503],
    "Q5":  [11034, 5517, 3848, 3574, 4651, 5346],
    "Q12": [12049,7717, 7081, 6476, 5841, 7059],
    "Q6":  [2789, 1310, 695, 530, 469, 485],
}

# ======================
# Cache miss
# bc = baseline cache miss
# oc = optimized cache miss
# ======================
cache_miss_b = {
    "Q3":  [31, 32, 34, 35, 37, 40],
    "Q5":  [31, 36, 40, 41, 44,50],
    "Q12": [31, 36, 41, 42, 44, 50],
    "Q6":  [41, 43, 44, 45, 45, 44],
}

cache_miss_o = {
    "Q3":  [32, 33, 33, 34, 36, 37],
    "Q5":  [32, 39, 42, 43, 43, 47],
    "Q12": [32, 34, 38, 41, 39, 42],
    "Q6":  [38, 39, 40, 41, 41, 40],
}

# ======================
# Compute Speedup (基于 DOP=8)
# ======================
speedup_b = {}
speedup_o = {}

for q in queries:
    base = exec_time_b[q][0]   # 统一用 baseline@8DOP

    speedup_b[q] = [base / t for t in exec_time_b[q]]
    speedup_o[q] = [base / t for t in exec_time_o[q]]

# ======================
# Compute Δ Cache Miss (基于 DOP=8)
# ======================
delta_miss_b = {}
delta_miss_o = {}

for q in queries:
    base_b = cache_miss_b[q][0]
    base_o = cache_miss_o[q][0]

    delta_miss_b[q] = [v - base_b for v in cache_miss_b[q]]
    delta_miss_o[q] = [v - base_o for v in cache_miss_o[q]]

# ======================
# Plot: 4 rows × 2 columns
# ======================
fig, axes = plt.subplots(
    4, 2,
    figsize=(8.5, 11),
    sharex=True
)

markers = {
    "Q3": "o",
    "Q5": "s",
    "Q12": "^",
    "Q6": "d",
}

# ======================
# Left column: Speedup
# ======================
for i, q in enumerate(queries):
    ax = axes[i, 0]

    ax.plot(
        dop, speedup_b[q],
        marker=markers[q],
        linewidth=2,
        label="Baseline"
    )

    ax.plot(
        dop, speedup_o[q],
        marker=markers[q],
        linewidth=2,
        linestyle="--",
        label="Optimized"
    )

    ax.set_title(q, fontsize=15)
    ax.set_ylabel("Scale Up", fontsize=14)
    ax.grid(axis="y", linestyle="--", alpha=0.3)
    ax.tick_params(labelsize=11)

    if i == 0:
        ax.legend(fontsize=11, loc="upper left")

# ======================
# Right column: Δ Cache Miss
# ======================
for i, q in enumerate(queries):
    ax = axes[i, 1]

    ax.plot(
        dop, delta_miss_b[q],
        marker=markers[q],
        linewidth=2
    )

    ax.plot(
        dop, delta_miss_o[q],
        marker=markers[q],
        linewidth=2,
        linestyle="--"
    )

    ax.axhline(0, linestyle="--", linewidth=1, alpha=0.5)
    ax.set_ylabel("Δ Cache Miss", fontsize=14)
    ax.grid(axis="y", linestyle="--", alpha=0.3)
    ax.tick_params(labelsize=11)

# ======================
# X-axis
# ======================
axes[-1, 0].set_xlabel("DOP", fontsize=14)
axes[-1, 1].set_xlabel("DOP", fontsize=14)

for ax in axes[-1]:
    ax.set_xticks(dop)

fig.align_ylabels()
plt.tight_layout()

# ======================
# Save as PDF
# ======================
plt.savefig(
    "tpch_baseline_vs_optimized_speedup_cachemiss.pdf",
    format="pdf",
    dpi=300,
    bbox_inches="tight"
)

plt.close()
