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

queries = ['Q4', 'Q3', 'Q10', 'Q5', 'Q7']
x = np.arange(len(queries))
width = 0.35

locality_16  = np.array([58, 56, 50, 62, 40]) / 100.0
locality_128 = np.array([26, 22, 23, 24, 48]) / 100.0

# -----------------------------
# Data volume (MB)
# -----------------------------
local_16   = np.array([272, 3201, 883, 1607, 4637])
remote_16  = np.array([842, 9208, 2643, 4989, 11972])

local_128  = np.array([701, 2802, 2791, 2086, 1774])
remote_128 = np.array([3058, 10712, 9806, 9723, 6752])

color_16  = '#87CEFA'   # Light Sky Blue
color_128 = '#FA8072'   # Salmon
edge_col  = '#4D4D4D'

fig, axes = plt.subplots(1, 2, figsize=(10, 4))

# ===== (a) Inter-Operator NUMA Locality =====
axes[0].bar(
    x - width/2, locality_16, width,
    label='16 DOP',
    color=color_16,
    edgecolor=edge_col,
    linewidth=0.6
)

axes[0].bar(
    x + width/2, locality_128, width,
    label='128 DOP',
    color=color_128,
    edgecolor=edge_col,
    linewidth=0.6
)

axes[0].set_xticks(x)
axes[0].set_xticklabels(queries)
axes[0].set_ylabel('NUMA Locality')
axes[0].set_xlabel('Query', labelpad=12)
axes[0].set_ylim(0, 1.0)

axes[0].text(
    0.5, -0.25,
    '(a) Inter-Operator NUMA Locality',
    transform=axes[0].transAxes,
    ha='center', va='top', fontsize=14
)

axes[0].legend(frameon=False, loc='upper left')

# ===== (b) Local vs. Remote Data Exchange =====
# --- 16 DOP ---
axes[1].bar(
    x - width/2, local_16, width,
    label='Local',
    color=color_16,
    edgecolor=edge_col,
    linewidth=0.6
)

axes[1].bar(
    x - width/2, remote_16, width,
    bottom=local_16,
    label='Remote',
    color=color_16,
    edgecolor=edge_col,
    linewidth=0.6,
    alpha=0.4
)

# --- 128 DOP ---
axes[1].bar(
    x + width/2, local_128, width,
    color=color_128,
    edgecolor=edge_col,
    linewidth=0.6
)

axes[1].bar(
    x + width/2, remote_128, width,
    bottom=local_128,
    color=color_128,
    edgecolor=edge_col,
    linewidth=0.6,
    alpha=0.4
)

axes[1].set_xticks(x)
axes[1].set_xticklabels(queries)
axes[1].set_ylabel('Data Volume (MB)')
axes[1].set_xlabel('Query', labelpad=12)

axes[1].text(
    0.5, -0.25,
    '(b) Local and Remote Data Exchange',
    transform=axes[1].transAxes,
    ha='center', va='top', fontsize=14
)

axes[1].legend(frameon=False, loc='upper left')


plt.tight_layout(pad=0.8, w_pad=1.2)

plt.show()

