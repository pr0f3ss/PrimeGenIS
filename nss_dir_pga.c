#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "nss_pga.h"

/* 
function: generates prime with nss method and exlusively Dirichlet sieve, passed on into bignum p, sieve algo has to be Dirichlet sieve
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, u = # max nss_iter rounds, r = number of primes to do trial division with, l = max deviation from output prime to trial 
returns: 1 if successful, 0 if failure, -1 if error
*/

int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l, int (*generate_sieve)(unsigned short*, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int), int sieve_sz){	
	BIGNUM *n;
	n = BN_new();

	int ret = -1; // return code of nss_iter
	int j = 0; // iteration var

	unsigned short *sieve = NULL;
    sieve = (unsigned short*) malloc(sizeof(short)*(sieve_sz)); 

	// tries at most u nss_iter steps
	while(ret != 1 && j < u){
		ret = nss_iter(n, k, r, t, l, generate_sieve, sieve_algo, sieve);
		j+=1;
	}

	if(ret == 1){
		if(!BN_copy(p, n)){ // copy value from n to p on successful prime generation
			ret = -1;
		} 
	}

	BN_free(n);
	free(sieve);

	return ret;
}


/*
function: represents one step of nss method, uses L to limit the number of m_r additions in the Dirichlet sieve
arguments: p = returned prime if successfully generated, k = bit size of prime, r = number of primes to do trial division with, t = # MR rounds, l = max deviation from output prime to trial
returns: 1 if successful, 0 if failure, -1 if error (sieve generation)
other:  l = max deviation from initially generated num and probable prime 
*/

int nss_iter(BIGNUM *p, int k, int r, int t, int l, int (*generate_sieve)(unsigned short*, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int), unsigned short* sieve){
	int ret = 0; // return code of nss_iter

	// create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// generate k-bit, odd number, aka trial value
	BIGNUM *n0;
	n0 = BN_new();

	// p is going to copy from this var on success
	BIGNUM *n;
	n = BN_new();

	/* ========= SIEVE GENERATION SECTION ============= */
	int sieve_sz = l/2;

	// generate sieve for nss_sieve method
	if(!generate_sieve(sieve, sieve_sz, n0, r)){
		ret = -1;
		goto free_bn;
	}

	/* ========= END SIEVE GENERATION SECTION ============= */

	unsigned long it = 0; // is passed on as an iterator variable inside nss_sieve to do the sieve checking. 
    int nr_add = 0;
	// pre-condition: n0 holds trial value
	do{
		ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);

		if(nr_add > l || ret != 1 || BN_num_bits(n) != k){
			ret = 0;
			goto free_bn;
		}

        nr_add++;

	}while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL)); // signature: int BN_is_prime_fasttest_ex(const BIGNUM *p, int nchecks, BN_CTX *ctx, int do_trial_division, BN_GENCB *cb);

	// post-condition: ret = 1, n passed t MR-rounds and n-n0 < l

	if(!BN_copy(p, n)){ // copy value from n to p on successful prime generation
		ret = -1;
	} 
		
	free_bn:
		BN_free(n);
		BN_free(n0);
		BN_CTX_free(ctx);

	return ret;
}