import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['font.family'] = 'Arial'
mpl.rcParams['pdf.fonttype'] = 42

# ======================
# DOP
# ======================
dop = np.array([8, 16, 32, 64, 96, 128])
x_eq = np.arange(len(dop)) 

queries = ["Q4", "Q3", "Q10", "Q5", "Q7"]

# ======================
# Execution time (baseline / optimized)
# ======================
time_data = {
    "Q3":  [[22297,11297,5970,5140,5262,6676],
            [21145,10088,5721,4997,4644,4203]],
    "Q4":  [[22085,10955,7440,5046,3947,3058],
            [17914,10822,8903,5207,3712,2890]],
    "Q5":  [[11034,5517,3848,3631,5623,6211],
            [7704,5024,3673,3847,5712,6328]],
    "Q7":  [[14274,8727,5828,5281,8021,11081],
            [13263,8012,5573,5396,6878,10079]],
    "Q10": [[13496,6748,4327,3349,4496,5914],
            [10287,5829,4498,2927,3171,4080]],
}

# ======================
# Remote NUMA traffic (MB) —— 更新后的数据
# ======================
remote_mb = {
    "Q3": {
        "baseline": [9208, 9337, 9983, 10542, 10894, 10712],
        "opt":      [3292, 1872, 2127, 4446, 7757, 9495],
    },
    "Q4": {
        "baseline": [820, 842, 2036, 2081, 2097, 3058],
        "opt":      [860, 907,1067, 1292, 1460, 1757],
    },
    "Q5": {
        "baseline": [4100, 4989, 7915, 15962, 9761, 9723],
        "opt":      [2032, 3778, 7465, 17711, 10819, 11512],
    },
    "Q7": {
        "baseline": [4098, 11972, 22051, 65639, 4575, 6752],
        "opt":      [1339, 1005, 2541, 64301, 4399, 5255],
    },
    "Q10": {
        "baseline": [2510, 2643, 2649, 6832, 6808, 9806],
        "opt":      [2039, 2950, 3090, 5329, 5803, 8716],
    },
}

# ======================
# Scale-up (baseline@8DOP)
# ======================
speedup = {}
for q in queries:
    base = time_data[q][0][0]
    speedup[q] = {
        "baseline": base / np.array(time_data[q][0]),
        "opt":      base / np.array(time_data[q][1]),
    }

# ======================
# Marker mapping
# ======================
markers = {
    "Q3": "o",
    "Q4": "s",
    "Q5": "^",
    "Q7": "d",
    "Q10": "v",
}

# ======================
# Figure
# ======================
fig, axes = plt.subplots(
    nrows=len(queries),
    ncols=2,
    figsize=(8.5, 13),
    sharex=False
)

bar_width = 0.35   # 等距坐标下的柱宽

# ======================
# Plot
# ======================
for i, q in enumerate(queries):

    # -------- Left: Scale-up (真实 DOP 轴) --------
    ax_l = axes[i, 0]
    ax_l.plot(dop, speedup[q]["baseline"],
              marker=markers[q], linewidth=2, label="Baseline")
    ax_l.plot(dop, speedup[q]["opt"],
              marker=markers[q], linewidth=2, linestyle="--", label="Optimized")

    ax_l.set_title(q, fontsize=15)
    ax_l.set_ylabel("Scale Up", fontsize=14)
    ax_l.grid(axis="y", linestyle="--", alpha=0.3)
    ax_l.tick_params(labelsize=11)

    if i == 0:
        ax_l.legend(fontsize=11, loc="upper left")

    ax_l.set_xticks(dop)
    ax_l.set_xticklabels(dop)
    # -------- Right: Remote NUMA traffic (等距柱形) --------
    ax_r = axes[i, 1]

    ax_r.bar(
        x_eq - bar_width/2,
        remote_mb[q]["baseline"],
        width=bar_width,
        label="Remote (Baseline)",
        alpha=0.85
    )

    ax_r.bar(
        x_eq + bar_width/2,
        remote_mb[q]["opt"],
        width=bar_width,
        label="Remote (Optimized)",
        alpha=0.85
    )

    ax_r.set_ylabel("Remote NUMA Traffic (MB)", fontsize=14)
    ax_r.set_xticks(x_eq)
    ax_r.set_xticklabels(dop)
    ax_r.grid(axis="y", linestyle="--", alpha=0.3)
    ax_r.tick_params(labelsize=11)

    if i == 0:
        ax_r.legend(fontsize=11, loc="upper right")

# ======================
# X-axis labels
# ======================
axes[-1, 0].set_xlabel("DOP", fontsize=14)
axes[-1, 1].set_xlabel("DOP", fontsize=14)

fig.align_ylabels()
plt.tight_layout()

# ======================
# Save
# ======================
plt.savefig(
    "tpch_scaleup_vs_remote_numa_mixed_axis.pdf",
    dpi=300,
    bbox_inches="tight"
)
plt.close()
