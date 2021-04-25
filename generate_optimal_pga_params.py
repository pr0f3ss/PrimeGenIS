from pga_params import nss_params
from pga_params import ossl_params
from pga_params import nat_params
import csv
import sys

if __name__ == "__main__":
	k=1024
	r_max = 8080
	l_max = 65536
	r_bound = 3024
	l_bound = 12000

	#nss param generator
	filepath = "data/optimal_params/nss_pga/"
	r=sys.argv[1]
	filename = filepath+"nss_k1024_r"+r+"_l700_l65536.csv"
	with open(filename, 'w', newline='') as file:
		writer = csv.writer(file)
		writer.writerow(["r", "l", "U", "t"])
		l = 700
		while(l <= l_max):
			l_inc = 1024 if l > l_bound else 512
			U, t = nss_params(k, l, r)
			writer.writerow([r, l, U, t])
			l += l_inc

	# #ossl param generator
	# filepath = "data/optimal_params/openssl_pga/"
	# filename = filepath+"openssl_k1024_r16_r8080.csv"
	# r=16
	# l=2
	# with open(filename, 'w', newline='') as file:
	#     writer = csv.writer(file)
	#     writer.writerow(["r", "t"])
	#     while(r <= r_max):
	#     	r_inc = 64 if r >= r_bound else 16
 #    		t = ossl_params(k, l, r)
 #    		writer.writerow([r, t])
	#     	r += r_inc
	# print("finished openssl param generation")

	#nat param generator
	# filepath = "data/optimal_params/nat_pga/"
	# filename = filepath+"nat_k1024_r16_r8080csv"
	# r=16
	# l=64000
	# with open(filename, 'w', newline='') as file:
	#     writer = csv.writer(file)
	#     writer.writerow(["r", "t"])
	#     while(r <= r_max):
	#     	r_inc = 64 if r >= r_bound else 16
    # 		t = nat_params(k, l, r)
    # 		writer.writerow([r, t])
	#     	r += r_inc
	# print("finished nat param generation")

