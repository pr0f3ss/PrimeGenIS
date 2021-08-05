# this file serves the purpose to compute sufficient hyperparameters such that
# the pgas meet error and failure probabilites that the security parameter sets
import math
import numpy as np
#import xlsxwriter,datetime,os

SECURITY_PARAM = 128

def prime(i, primes):
    for prime in primes:
        if not (i == prime or i % prime):
            return False
    primes.add(i)
    return i

def historic(n):
    primes = set([2])
    i, p = 2, 0
    while True:
        if prime(i, primes):
            p += 1
            if p == n:
                return primes
        i += 1

# Courtesy of Mia Filic and her team
# Due to Brandt /Damgard paper we are able to upper bound the number of rounds needed for the IS PGA (MRt) algorithm to reach certain error probability.

def qkts(k=1024, L=1024*np.log(2)*10, t=1, M = -1):
    Cm_sum = []
    for m in range(3,M+1):
        Cm_sum.append(8.0/3.0*(np.pi**2 -6.0)*np.sum([np.power(2, m-j-(k-1.0)/j)for j in range(2,m+1)])*(2**(-t*(m-1))))
    c = L/(2*k*np.log(2))
    q = 0.5*(c*k)**2 * np.sum(Cm_sum) + 0.7*c*k*(2**(-t*M))
    return q

def retrieve_optimal_params(k, t_list, L_list, pga_spec):
    out = np.empty(shape=(len(t_list), len(L_list)))
    for i,t in enumerate(t_list):
        for j,L in enumerate(L_list):
            M_max = int(2*np.sqrt(k-1)-1)+1
            q_list = [qkts(k,L,t,M) for M in range(M_max)]
            M_min = np.where(q_list == np.min(q_list))
            p_inc_search_log2 = np.log2(q_list[M_min[0][0]])
            out[i][j] = -p_inc_search_log2
    
    for i in range(0, out.shape[0]):
        for j in range(0, out.shape[1]):
            if(out[i][j] + pga_spec > SECURITY_PARAM + 1):
                return t_list[i], L_list[j]
    
    print("No t and l compositions found to satisfy error probability. Consider increasing t or decreasing L.")

# input: k = bitsize, l = max deviation, r = number of primes
# output: U = nss_iter rounds, t = MR-rounds 
def nss_params(k, l, r):
    U = math.ceil((SECURITY_PARAM * k * math.log(2) * math.log(2))/l)
    t_list = [i for i in range(1,20)]
    L_list = [l]

    lhs = k*math.log(2)/2
    rhs = (1 - math.exp(-l * U / k * math.log(2))) / (1 - math.exp(-l / k * math.log(2)))
    pga_spec = -math.log2(min(lhs, rhs))
    t, l = retrieve_optimal_params(k, t_list, L_list, pga_spec)
    return U, t

# input: k = bitsize, l = max deviation, r = number of primes 
# output: t = MR-rounds
def ossl_params(k, l, r):
    t_list = [i for i in range(1,20)]
    L_list = [l]

    primesum = 0
    primes = sorted(list(historic(r)))
    for prime in primes:
        primesum += 1 + (1/(prime-1))
    primesum = 2 * primesum
        
    pga_spec = -math.log2((k*math.log(2)/primesum))
    t, l = retrieve_optimal_params(k, t_list, L_list, pga_spec)
    return t

# input: k = bitsize, l = max deviation, r = number of primes 
# output: t = MR-rounds
def nat_params(k, l, r):
    # e^-2c <= 2^-g
    # -2c * log2(e) <= -g
    # c > g / (log2(e)*2)
    # c = 45
    c = math.ceil(SECURITY_PARAM / (math.log2(math.e) * 2))
    if(l < (2*c*k*math.log(2))):
        print("Supplied L too small to pass failure rate.")
        return -1

    t_list = [i for i in range(1,20)]
    L_list = [l]
    pga_spec = 0
    t, l = retrieve_optimal_params(k, t_list, L_list, pga_spec)
    return t


if __name__ == "__main__":
    # test
    k = 1024
    l = 4000
    r = 1024

    # test nss
    # U, t = nss_params(k,l,r)
    # print("NSS: U={}, t={}".format(U, t))

    # # test ossl
    # t = ossl_params(k,l,r)
    # print("OSSL: t={}".format(t))

    # # test nat
    # t = nat_params(k,64000,r) #boundary: 63,880
    # print("NAT: t={}".format(t))

    U, t = nss_params(k, 38000, 16)
    print("NSS: U={}, t={}".format(U, t))