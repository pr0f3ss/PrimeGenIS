#ifndef NAT_PGA_HEADER
#define NAT_PGA_HEADER

int openssl_pga(BIGNUM *p, int k, int t, int r, int l);

int openssl_iter(BIGNUM *p, int k, int r, int t, int l);

int openssl_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it);

int openssl_generate_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n0, int r);

#endif