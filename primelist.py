import json

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


if __name__ == "__main__":
    with open('firstprimes.txt', 'w') as filehandle:
        json.dump(sorted(list(historic(8192))), filehandle)