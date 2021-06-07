#ifndef SAFE_NAT_PGA_HEADER
#define SAFE_NAT_PGA_HEADER

int safe_nat_nss_pga(BIGNUM *p, int k, int t, int r, int l);
int safe_nat_nss_sieve_init(unsigned short *sieve, int sieve_sz, BIGNUM *n0, int r);
int safe_nat_nss_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k);

#endif