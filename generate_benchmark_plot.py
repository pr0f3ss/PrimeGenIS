import numpy as np
import matplotlib.pyplot as plt
from numpy import genfromtxt

csv_data = genfromtxt('data/dirichlet_benchmark/dirichlet_nss_analysis_r33.csv', delimiter=',') #r, l, runtime for 8192 primes

l = csv_data[1:,1]
time = csv_data[1:,2]


plt.xlabel('l (max deviation)')
plt.ylabel('Runtime [s]')
plt.title('Average runtime for k=1024, r=33');


plt.plot(l, time)


plt.savefig("plots/dirichlet_nss_analysis_r33.png")

