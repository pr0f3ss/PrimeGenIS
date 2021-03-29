#ifndef NAT_PGA_HEADER
#define NAT_PGA_HEADER

int nat_pga(BIGNUM *p, int k, int t, int r, int l, int (*generate_sieve)(unsigned char*, int, BIGNUM, int), int (*sieve_algo)(unsigned char*, int, BIGNUM, BIGNUM, int, unsigned long*, int));

#endif