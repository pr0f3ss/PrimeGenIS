import numpy as np
import matplotlib.pyplot as plt
from numpy import genfromtxt

csv_data = genfromtxt('nss_analysis.csv', delimiter=',') #r, l, runtime for 512 primes

r = csv_data[1:,0]
l = csv_data[1:,1]
time = csv_data[1:,2]

linestyle = [':', '--', '-', '-.', '-']

plt.xlabel('l (max deviation)')
plt.ylabel('Runtime [s]')
plt.title('Runtime for 512 generated primes');

for i in range(0,5):
	plt.plot(l[i*5:((i*5)+5)],time[i*5:((i*5)+5)], linestyle[i])

plt.legend( [ 'r = 512'
            , 'r = 1024'  
            , 'r = 2048' 
            , 'r = 4096'      
            , 'r = 8192' 
            ] )

plt.savefig("plots/nss_analysis_pic.png")

