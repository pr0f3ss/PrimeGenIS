#ifndef NSS_PGA_HEADER
#define NSS_PGA_HEADER

int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l, int (*generate_sieve)(unsigned short**, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int));

int nss_iter(BIGNUM *p, int k, int r, int t, int l, int (*generate_sieve)(unsigned short**, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int));

int nss_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k);

int nss_generate_sieve(unsigned short **sieve, int sieve_sz, BIGNUM *n0, int r);

#endif