import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['font.family'] = 'Arial'
mpl.rcParams['font.size'] = 14
mpl.rcParams['axes.labelsize'] = 14
mpl.rcParams['axes.titlesize'] = 14
mpl.rcParams['legend.fontsize'] = 11
mpl.rcParams['pdf.fonttype'] = 42

# ======================
# DOP setup
# ======================
dops = np.array([8, 16, 32, 64, 96, 128])
x = np.arange(len(dops))  # unified x-axis

queries = ["Q4", "Q14", "Q3", "Q5", "Q10", "Q21"]

# ======================
# Raw execution time
# ======================
raw_time = {
    "Q3":  {"baseline": [22297,11297,5570,4240,4762,6676],
            "opt":      [23064,12064,5470,4196,3857,4126]},
    "Q4":  {"baseline": [22020,10955,7440,5046,3947,3058],
            "opt":      [21820,10855,7174,4725,2960,2544]},
    "Q5":  {"baseline": [11034,5517,3848,3631,5623,6211],
            "opt":      [10054,5002,3552,3259,4978,5097]},
    "Q10": {"baseline": [13568,6748,4327,3349,4496,5914],
            "opt":      [9593,4773,4123,3061,3182,4581]},
    "Q14": {"baseline": [3503,1743,992,811,1056,1004],
            "opt":      [3680,1831,1024,800,1193,1321]},
    "Q21": {"baseline": [86693,43130,25963,16758,14902,12429],
            "opt":      [42029,20910,11879,9975,9408,8066]},
}

# ======================
# Scale-up (baseline@8)
# ======================
speedup = {}
for q in queries:
    t8 = raw_time[q]["baseline"][0]
    speedup[q] = {
        "baseline": t8 / np.array(raw_time[q]["baseline"]),
        "opt":      t8 / np.array(raw_time[q]["opt"]),
    }

# ======================
# Cross-NUMA migration
# ======================
cross_numa = {
    "Q3":  [478, 318, 56785, 486582, 812341, 1335089],
    "Q4":  [195, 603, 12819, 242211, 401233, 612840],
    "Q5":  [88, 211, 6241, 183445, 299120, 447892],
    "Q10": [104, 388, 4320, 95221, 164388, 298174],
    "Q14": [56, 182, 2224, 106641, 173920, 266401],
    "Q21": [6157, 63438, 596959, 1335089, 2019440, 3012103],
}

markers = {
    "Q3": "o", "Q4": "s", "Q5": "^",
    "Q10": "v", "Q14": "d", "Q21": "P"
}

# ======================
# Figure
# ======================
fig, axes = plt.subplots(
    nrows=len(queries),
    ncols=2,
    figsize=(9.0, 13.5),
    gridspec_kw={"width_ratios": [1.0, 1.0]}
)

# ======================
# Plot
# ======================
for i, q in enumerate(queries):

    # -------- Left: Scale-up --------
    ax_l = axes[i, 0]
    ax_l.plot(x, speedup[q]["baseline"],
              marker=markers[q], linewidth=2, label="Baseline")
    ax_l.plot(x, speedup[q]["opt"],
              marker=markers[q], linestyle="--", linewidth=2, label="Optimized")

    ax_l.set_title(q, loc="left")
    ax_l.set_ylabel("Scale-up")
    ax_l.set_xticks(x)
    ax_l.set_xticklabels(dops)
    ax_l.grid(axis="y", linestyle="--", alpha=0.3)

    if i == 0:
        ax_l.legend(frameon=False, loc="upper left")

    # -------- Right: Cross-NUMA --------
    ax_r = axes[i, 1]
    ax_r.bar(
        x, cross_numa[q],
        width=0.6,
        edgecolor="black",
        linewidth=0.6
    )

    ax_r.set_yscale("log")
    ax_r.set_ylabel("Migrations")
    ax_r.set_xticks(x)
    ax_r.set_xticklabels(dops)
    ax_r.grid(axis="y", linestyle="--", alpha=0.3)

    # annotate 16 DOP
    idx_16 = np.where(dops == 16)[0][0]
    ax_r.text(
        x[idx_16],
        cross_numa[q][idx_16],
        f"{cross_numa[q][idx_16]}",
        ha="center",
        va="bottom",
        fontsize=10
    )

# ======================
# X labels
# ======================
axes[-1, 0].set_xlabel("DOP")
axes[-1, 1].set_xlabel("DOP")

fig.align_ylabels()
plt.tight_layout()

# ======================
# Save
# ======================
plt.savefig("tpch_scaleup_vs_crossnuma.pdf",
            dpi=300, bbox_inches="tight")
plt.close()
