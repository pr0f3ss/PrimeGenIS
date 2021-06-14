#ifndef SAFE_DIRICHLET_SIEVE_HEADER
#define SAFE_DIRICHLET_SIEVE_HEADER

int safe_dirichlet_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k);

int safe_dirichlet_generate_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n0, int r);

#endif