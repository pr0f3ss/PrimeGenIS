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

int nat_pga(BIGNUM *p, int k, int t, int r, int l, int (*generate_sieve)(unsigned char**, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int)){
    int ret = -1;


    // create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

    // create trial
    BIGNUM *n0;
	n0 = BN_new();
	BN_rand(n0, k, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD);

    // initialize returned value from sieve
    BIGNUM *n;
	n = BN_new();

    /* SIEVE */
    unsigned char *sieve;
    int sieve_sz = l/2;

    if(!generate_sieve(&sieve, sieve_sz, n0, r)){
        BN_free(n0);
        BN_free(n);
        BN_CTX_free(ctx);
        free(sieve);
        return -1;
    }

    unsigned long it = 0; // is passed on as an iterator variable inside openssl_sieve to do the sieve checking. 

    ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);
    /* SIEVE */

    // check for bit length of returned n
    if(ret != 1 || BN_num_bits(n) != k){

        BN_free(n0);
        BN_free(n);
        BN_CTX_free(ctx);
        free(sieve);

        return ret;
    }

    // pre-condition: n is odd, k-bit long
    while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL)){
        
        //int nat_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it)
        ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);
        
        // check bit length of returned n
        if(ret!= 1 || BN_num_bits(n) != k){
            BN_free(n0);
            BN_free(n);
            BN_CTX_free(ctx);
            free(sieve);
            return ret;
        }
    }
    // post-condition: n passed t MR-rounds and trial division, so probable prime => return value set to 1 
    
    ret = 1;
	BN_copy(p, n); // copy value from n to p on successful prime generation

    BN_free(n0);
	BN_free(n);
    BN_CTX_free(ctx);
    free(sieve);

	return ret;
}

