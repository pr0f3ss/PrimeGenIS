#ifndef NSS_PGA_HEADER
#define NSS_PGA_HEADER


int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);

int nss_iter(BIGNUM *p, int k, int r, int t, int l);

int nss_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it);

int nss_generate_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n0, int r);

#endif