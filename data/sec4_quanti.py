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
queries = ['Q6', 'Q3', 'Q12', 'Q5']

# -----------------------------
# IOI 数据
# -----------------------------
ioi_16  = np.array([0.00, 0.55, 0.47, 0.53])
ioi_128 = np.array([0.00, 0.72, 0.85, 0.83])
# -----------------------------
miss_16  = np.array([0.23, 0.17, 0.40, 0.32])
miss_128 = np.array([0.25, 0.29, 0.54, 0.51])
color_16  = '#87CEFA'   # Light Sky Blue
color_128 = '#FA8072'   # Salmon
edge_col  = '#4D4D4D'

fig, axes = plt.subplots(1, 2, figsize=(10, 4))
width = 0.35
x = np.arange(len(queries))

# ===== (a) Inter-Operator Interference (IOI) =====
axes[0].bar(x - width/2, ioi_16, width,
            label='16 DOP', color=color_16,
            edgecolor=edge_col, linewidth=0.6)
axes[0].bar(x + width/2, ioi_128, width,
            label='128 DOP', color=color_128,
            edgecolor=edge_col, linewidth=0.6)

axes[0].set_xticks(x)
axes[0].set_xticklabels(queries)
axes[0].set_ylabel('IOI')
axes[0].set_xlabel('Query', labelpad=12)
axes[0].set_ylim(0, 1.0)

axes[0].text(0.5, -0.25,
             '(a) Inter-Operator Interference (IOI)',
             transform=axes[0].transAxes,
             ha='center', va='top', fontsize=14)

axes[0].legend(frameon=False, loc='upper left')
# --- 标注 Q6: IOI = 0 (single operator) ---
q6_idx = 0  # Q6 在 queries 中的索引

axes[0].text(
    q6_idx, 0.02,
    '0\n(single operator)',
    ha='center', va='bottom',
    fontsize=12
)

# ===== (b) LLC Cache Miss Rate =====
axes[1].bar(x - width/2, miss_16, width,
            label='16 DOP', color=color_16,
            edgecolor=edge_col, linewidth=0.6)
axes[1].bar(x + width/2, miss_128, width,
            label='128 DOP', color=color_128,
            edgecolor=edge_col, linewidth=0.6)

axes[1].set_xticks(x)
axes[1].set_xticklabels(queries)
axes[1].set_ylabel('Cache Miss Rate')
axes[1].set_xlabel('Query', labelpad=12)
axes[1].set_ylim(0, 1.0)

axes[1].text(0.5, -0.25,
             '(b) LLC Cache Miss Rate',
             transform=axes[1].transAxes,
             ha='center', va='top', fontsize=14)

axes[1].legend(frameon=False, loc='upper left')

# -----------------------------
# 布局 & 保存（保持不变）
# -----------------------------
plt.tight_layout(pad=0.8, w_pad=1.2)

plt.show()

fig.savefig('query_ioi_cache_miss.pdf', bbox_inches='tight')
fig.savefig('query_ioi_cache_miss.png', dpi=300, bbox_inches='tight')
