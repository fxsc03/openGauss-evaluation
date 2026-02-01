import matplotlib.pyplot as plt

# ======================
# Common X axis
# ======================
dop = [4, 8, 16, 32, 64, 96, 128]

# ======================
# Execution time (ms)
# ======================
exec_time = {
    "Q3": [42, 24, 16, 9, 6, 4, 5.7],
    "Q5": [29, 16, 10, 7, 5, 6, 5],
    "Q1": [234, 120, 67, 46, 36, 38, 36],
    "Q6": [12, 6, 3.4, 1.6, 1, 0.89, 0.67],
}

# ======================
# Cache miss
# ======================
cache_miss = {
    "Q3": [33, 30, 32, 34, 35, 37, 40],
    "Q5": [27, 31, 36, 41, 42, 44, 50],
    "Q1": [36, 37, 35, 39, 50, 56, 45],
    "Q6": [41, 43, 45, 42, 45, 42, 35],
}

queries = ["Q3", "Q5", "Q1", "Q6"]

# ======================
# Compute speedup
# ======================
speedup = {}
for q, times in exec_time.items():
    base = times[0]
    speedup[q] = [base / t for t in times]

# ======================
# Compute delta cache miss
# ======================
delta_miss = {}
for q, values in cache_miss.items():
    base = values[0]
    delta_miss[q] = [v - base for v in values]

# ======================
# Create figure: 4 rows × 2 columns
# ======================
fig, axes = plt.subplots(
    4, 2,
    figsize=(7.5, 10.5),
    sharex=True
)

markers = {
    "Q3": "o",
    "Q5": "s",
    "Q1": "^",
    "Q6": "d",
}

# ======================
# Left column: Speedup
# ======================
for i, q in enumerate(queries):
    ax = axes[i, 0]
    ax.plot(
        dop, speedup[q],
        marker=markers[q],
        linewidth=2
    )
    ax.set_title(q, fontsize=16)
    ax.set_ylabel("Speedup", fontsize=16)
    ax.grid(axis="y", linestyle="--", alpha=0.3)
    ax.tick_params(labelsize=12)

# ======================
# Right column: Δ Cache Miss
# ======================
for i, q in enumerate(queries):
    ax = axes[i, 1]
    ax.plot(
        dop, delta_miss[q],
        marker=markers[q],
        linewidth=2
    )
    ax.axhline(0, linestyle="--", linewidth=1, alpha=0.5)
    ax.set_ylabel("Δ Cache Miss", fontsize=16)
    ax.grid(axis="y", linestyle="--", alpha=0.3)
    ax.tick_params(labelsize=12)

# ======================
# X-axis labels
# ======================
axes[-1, 0].set_xlabel("DOP", fontsize=16)
axes[-1, 1].set_xlabel("DOP", fontsize=16)

for ax in axes[-1]:
    ax.set_xticks(dop)
    
fig.align_ylabels()
 
# ======================
# Layout & save
# ======================
plt.tight_layout()
plt.savefig(
    "tpch_speedup_delta_cache_miss_vertical.pdf",
    format="pdf",
    dpi=300,
    bbox_inches="tight"
)
plt.close()
