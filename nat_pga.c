#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "nat_pga.h"

/* 
function: generates prime with natural algorithm, passed on into bignum p
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, r = number of primes to do trial division with, l = max deviation from output prime to trial 
returns: 1 if successful, 0 if failure, -1 if error
*/

int nat_pga(BIGNUM *p, int k, int t, int r, int l, int (*generate_sieve)(unsigned short*, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int), int sieve_sz){
    
    int ret = 0;

    // create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

    // create trial
    BIGNUM *n0;
	n0 = BN_new();

    // initialize returned value from sieve
    BIGNUM *n;
	n = BN_new();

    unsigned short *sieve = NULL;
    sieve = (unsigned short*) malloc(sizeof(short)*(sieve_sz)); 

    if(!BN_rand(n0, k, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD)){
		ret = -1;
        goto free_bn; // sieve hasn't been alloc'ed yet
    }

    if(!generate_sieve(sieve, sieve_sz, n0, r)){
        ret = -1;
        goto free_bn;
    }

    unsigned long it = 0; // is passed on as an iterator variable inside openssl_sieve to do the sieve checking. 

    ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);

    // check for bit length of returned n
    if(ret != 1 || BN_num_bits(n) != k){
        ret = 0;
        goto free_bn;
    }

    // pre-condition: n is odd, k-bit long
    while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL)){
        
        ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);
        
        // check bit length of returned n
        if(ret != 1 || BN_num_bits(n) != k){
            ret = 0;
            goto free_bn;
        }
    }
    // post-condition: n passed t MR-rounds, so probable prime 

	if(!BN_copy(p, n)){ // copy value from n to p on successful prime generation
        ret = -1;
        goto free_bn;
    } 

    free_bn:
        free(sieve);
        BN_free(n0);
        BN_free(n);
        BN_CTX_free(ctx);
    
	return ret;
}

