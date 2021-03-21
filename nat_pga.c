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

int nat_pga(BIGNUM *p, int k, int t, int r, int l){
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

	//int nat_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it){
    
    // check for bit length of returned n
    if(BN_num_bits(n) != k){

        BN_free(n0);
        BN_free(n);
        BN_CTX_free(ctx);

        return ret;
    }

    // holds value '2', used to increment n in loop
    BIGNUM *two_bn;
	two_bn = BN_new();
    unsigned long two_ul = 2;
    BN_set_word(two_bn, two_ul);

    // pre-condition: n is odd, k-bit long
    while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL)){
        BN_add(n, n, two_bn); // n = n+2
        
        //int nat_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it)
        // ret = sieve(); 

        // check bit length of returned n
        if(BN_num_bits(n) != k){
            BN_free(n0);
            BN_free(n);
            BN_free(two_bn);
            BN_CTX_free(ctx);
            return ret;
        }
    }
    // post-condition: n passed t MR-rounds and trial division, so probable prime => return value set to 1 
    
    ret = 1;
	BN_copy(p, n); // copy value from n to p on successful prime generation

    BN_free(n0);
	BN_free(n);
    BN_free(two_bn);
    BN_CTX_free(ctx);

	return ret;
}

