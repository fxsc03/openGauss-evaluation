import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['font.family'] = 'Arial'
mpl.rcParams['font.size'] = 14
mpl.rcParams['axes.labelsize'] = 14
mpl.rcParams['xtick.labelsize'] = 14
mpl.rcParams['ytick.labelsize'] = 14
mpl.rcParams['legend.fontsize'] = 14
mpl.rcParams['pdf.fonttype'] = 42
mpl.rcParams['ps.fonttype'] = 42

# -----------------------------
# Data
# -----------------------------
queries = ['Q3', 'Q4', 'Q5', 'Q10', 'Q14', 'Q21']
x = np.arange(len(queries))
width = 0.32

# Cross-NUMA migration counts
cross_16_raw = np.array([318, 603, 211, 388, 182, 63438])
cross_128_raw = np.array([1335089, 612840, 447892, 298174, 266401, 3012103])

cross_16 = cross_16_raw / 1e6
cross_128 = cross_128_raw / 1e6

# Ping-pong thread fraction
pp_16 = np.array([0.40, 0.23, 0.18, 0.50, 0.06, 0.43])
pp_128 = np.array([0.85, 0.62, 0.91, 0.86, 0.60, 0.99])

# -----------------------------
# Colors
# -----------------------------
color_16 = '#87CEFA'
color_128 = '#FA8072'
edge_col = '#4D4D4D'

# -----------------------------
# Plot
# -----------------------------
fig, axes = plt.subplots(1, 2, figsize=(9.2, 3.6))

# ===== Left: Cross-NUMA migration count =====
bars_16 = axes[0].bar(
    x - width/2, cross_16, width,
    label='16 DOP', color=color_16,
    edgecolor=edge_col, linewidth=0.6
)

bars_128 = axes[0].bar(
    x + width/2, cross_128, width,
    label='128 DOP', color=color_128,
    edgecolor=edge_col, linewidth=0.6
)

axes[0].set_xticks(x)
axes[0].set_xticklabels(queries)
axes[0].set_ylabel('Migrate Times (Million)')
axes[0].set_xlabel('Query')
axes[0].legend(frameon=False)
axes[0].grid(axis='y', linestyle='--', alpha=0.7)

# <<< 标注 16 DOP 的真实数值
for i, bar in enumerate(bars_16):
    height = bar.get_height()
    axes[0].text(
        bar.get_x() + bar.get_width() / 2,
        height + 0.02,
        f'{cross_16_raw[i]:,}',
        ha='center',
        va='bottom',
        fontsize=11,
        color='#333333'
    )

axes[0].text(
    0.5, -0.30,
    '(a) Cross-NUMA Thread Migrations',
    transform=axes[0].transAxes,
    ha='center', va='top'
)

# ===== Right: Ping-pong prevalence =====
axes[1].bar(
    x - width/2, pp_16, width,
    label='16 DOP', color=color_16,
    edgecolor=edge_col, linewidth=0.6
)

axes[1].bar(
    x + width/2, pp_128, width,
    label='128 DOP', color=color_128,
    edgecolor=edge_col, linewidth=0.6
)

axes[1].set_xticks(x)
axes[1].set_xticklabels(queries)
axes[1].set_ylabel('Ping-pong prevalence')
axes[1].set_xlabel('Query')
axes[1].set_ylim(0, 1.05)
axes[1].legend(frameon=False)
axes[1].grid(axis='y', linestyle='--', alpha=0.7)

axes[1].text(
    0.5, -0.30,
    '(b) Ping-Pong Prevalence',
    transform=axes[1].transAxes,
    ha='center', va='top'
)

plt.tight_layout(pad=0.8, w_pad=1.6)