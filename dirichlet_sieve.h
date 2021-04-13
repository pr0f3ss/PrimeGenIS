#ifndef DIRICHLET_SIEVE_HEADER
#define DIRICHLET_SIEVE_HEADER

int dirichlet_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k);

int dirichlet_generate_sieve(unsigned char **sieve, int sieve_sz, BIGNUM *n0, int r);

#endif