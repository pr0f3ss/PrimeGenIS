#ifndef SAFE_NAT_PGA_HEADER
#define SAFE_NAT_PGA_HEADER

int safe_nat_pga(BIGNUM *p, int k, int t, int r, int l, int (*generate_sieve)(unsigned short**, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int));

#endif