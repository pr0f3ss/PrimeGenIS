import numpy as np
import math

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

securityparam = 80
r = 2048      
k = 1024
ln2 = math.log(2)

primesum = 0
primes = sorted(list(historic(r)))
for prime in primes:
    primesum += 1 + (1/(prime-1))
primesum = 2 * primesum

c = securityparam+1 + math.log2(k*ln2/primesum) * ln2
l = 2*c*k*ln2
u = securityparam*k*ln2*ln2/l

#print("c=%f\nl=%f\nu=%f" % (c, l, u))
# nss rhs-term
print(np.log2([k*ln2/2, (1-math.exp(-(l*u)/(k*ln2)))/(1-math.exp(-l/(k*ln2)))]))