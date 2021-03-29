import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from glob import glob
from mpl_toolkits import mplot3d

stock_files = sorted(glob("data/nss_benchmark/nss_analysis_r*.csv"))

df = pd.concat((pd.read_csv(file) for file in stock_files), ignore_index = True)

df.columns = df.columns.str.strip()

r = df.r
l = df.l
avgruntime = df.avgruntime

ax = plt.axes(projection='3d')
ax.set_title("NSS benchmark on 8192 samples")
ax.set_xlabel('r')
ax.set_ylabel('l')
ax.set_zlabel('average runtime')

ax.plot_trisurf(r, l, avgruntime, cmap="coolwarm", edgecolor="none")

plt.savefig("benchmark/plots/nss_benchmark_plot.png")