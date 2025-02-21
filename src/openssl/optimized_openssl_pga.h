#ifndef OPT_OPENSSL_PGA_HEADER
#define OPT_OPENSSL_PGA_HEADER

int optimized_openssl_pga(BIGNUM *p, int k, int t, int r, int l);

int optimized_openssl_iter(BIGNUM *p, int k, int r, int t, int l, unsigned short* sieve);

int optimized_openssl_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k);

int optimized_openssl_generate_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n0, int r);

#endif