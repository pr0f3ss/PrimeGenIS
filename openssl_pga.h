#ifndef OPENSSL_PGA_HEADER
#define OPENSSL_PGA_HEADER

int openssl_pga(BIGNUM *p, int k, int t, int r, int l, int (*generate_sieve)(unsigned char*, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int));

int openssl_iter(BIGNUM *p, int k, int r, int t, int l, int (*generate_sieve)(unsigned char*, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int));

int openssl_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k);

int openssl_generate_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n0, int r);

#endif