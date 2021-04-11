#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"

/*
function: implements the dirichlet sieve, where if it=0 outputs n = z*mr + a where n in [2^(k-1), 2^k - 1], else outputs n = n+mr (n+n0)

z*mr+a in [2^(k-1), 2^k -1]
z*mr in [2^(k-1)-a, 2^k -(a+1)] where all interval values are divisible by mr

bn_rand_range(n0, (2^k - (2^(k-1) + 1))
n0 + 2^(k-1)-a
=> z in [2^(k-1)-a / mr, 2^k -(a+1) / mr]
=> z*mr+a in [2^(k-1), 2^k-1]

arguments: sieve = not used, sieve_sz = not used, n = z*mr+1 (with crypto rng) or n=n+mr (when it>0), n0 = mr (product of first r odd primes), r = number of primes to do trial division with, 
returns: 1 if successful, 0 if failure, -1 if error 
*/

int dirichlet_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k){
    int ret = 1;

    if(*it == 0){

        // create buffer for internal computations
        BN_CTX *ctx;
        ctx = BN_CTX_new();

        // initialize constant '1'
        BIGNUM *bn_one;
        bn_one = BN_new();
        BN_set_word(bn_one, (BN_ULONG) 1);

        // initialize a
        BIGNUM *bn_a;
        bn_a = BN_new();

        // holds gcd value
        BIGNUM *bn_gcd;
        bn_gcd = BN_new();
        
        // this do-while loop generates 'a' relatively prime to mr (gcd(mr,a) = 1)
        do{
            if(!BN_rand_range(bn_a, n0)){ // generate number in [0, mr]
                BN_free(bn_one);
                BN_free(bn_a);
                BN_free(bn_gcd);
                BN_CTX_free(ctx);
                return -1;
            }
            
            if(!BN_add(bn_a, bn_a, bn_one){ // bn_a in [1, mr+1]
                BN_free(bn_one);
                BN_free(bn_a);
                BN_free(bn_gcd);
                BN_CTX_free(ctx);
                return -1;
            } 

            if(!BN_gcd(bn_gcd, n0, bn_a, ctx)){ // compute gcd(n0, bn_a)
                BN_free(bn_one);
                BN_free(bn_a);
                BN_free(bn_gcd);
                BN_CTX_free(ctx);
                return -1;
            }
        }while(!BN_is_one(bn_gcd));

        //post-cond: a is relatively prime to mr
        BN_free(bn_gcd);

        // initialize lower interval bound = 2^(k-1) + 1, adding bn_one later down
        BIGNUM *bn_lw;
        bn_lw = BN_new();
        BN_set_word(bn_lw, (BN_ULONG) 0);
        BN_set_bit(bn_lw, (k-1));

        // initialize value for shifting rng interval = 2^(k-1) - a
        BIGNUM *bn_shift_interval;
        bn_shift_interval = BN_new();

        // set the values
        BN_sub(bn_shift_interval, bn_lw, bn_a); // = 2^(k-1) - a
        BN_add(bn_lw, bn_lw, bn_one); // = 2^(k-1) + 1

        // initialize upper interval bound = 2^k
        BIGNUM *bn_up;
        bn_up = BN_new();
        BN_set_word(bn_up, (BN_ULONG) 0);
        BN_set_bit(bn_up, k);

        BN_sub(bn_up, bn_up, bn_lw); // = (2^k - (2^(k-1) + 1))

        // initialize bignum holding remainder
        BIGNUM *rem;
        rem = BN_new();
        
        if(!BN_rand_range(n, bn_up)){ // generate number in [0, (2^k - (2^(k-1) + 1)]
            BN_free(bn_one);
            BN_free(bn_a);
            BN_free(bn_lw);
            BN_free(bn_up);
            BN_free(bn_shift_interval);
            BN_free(rem);
            BN_CTX_free(ctx);
            return -1;
        }

        // precondition: n is out of interval [0, (2^k - (2^(k-1) + 1)]

        if(!BN_add(n, n, bn_shift_interval)){ // shift the interval from [0, (2^k - (2^(k-1) + 1)] to [2^(k-1) - a, 2^k - (a+1)]
            BN_free(bn_one);
            BN_free(bn_a);
            BN_free(bn_lw);
            BN_free(bn_up);
            BN_free(bn_shift_interval);
            BN_free(rem);
            BN_CTX_free(ctx);
            return -1;
        }

        if(!BN_mod(rem, n, n0, ctx)){ // get remainder of n/mr, s.t. we have a z for which holds: n = z*mr
            BN_free(bn_one);
            BN_free(bn_a);
            BN_free(bn_lw);
            BN_free(bn_up);
            BN_free(bn_shift_interval);
            BN_free(rem);
            BN_CTX_free(ctx);
            return -1;
        }

        if(!BN_sub(n, n, rem)){ // subtract the remainder, s.t. n divisible by mr
            BN_free(bn_one);
            BN_free(bn_a);
            BN_free(bn_lw);
            BN_free(bn_up);
            BN_free(bn_shift_interval);
            BN_free(rem);
            BN_CTX_free(ctx);
            return -1;
        }

        // postcondition: n is out of interval [2^(k-1) - a, 2^k - (a+1)] AND n is divisible by mr which implies n = z*mr for some z

        if(!BN_add(n, n, bn_a)){ // construct n = z*mr + a
            BN_free(bn_one);
            BN_free(bn_a);
            BN_free(bn_lw);
            BN_free(bn_up);
            BN_free(bn_shift_interval);
            BN_free(rem);
            BN_CTX_free(ctx);
            return -1;
        }

        BN_free(bn_one);
        BN_free(bn_a);
        BN_free(bn_lw);
        BN_free(bn_up);
        BN_free(bn_shift_interval);
        BN_free(rem);
        BN_CTX_free(ctx);

        *it = 1; // set it=1 to signal that next iteration should just add mr
    }else{
        if(!BN_add(n, n, n0)){ // n = n+mr
            return -1;
        }
    }

    return 1;
}

/*
function: calculates mr for usage in the dirichlet sieve
arguments: sieve = not used, sieve_sz = not used, n0 = mr if successful, r = number of primes to do trial division with 
returns: 1 if successful, 0 if error 
*/
/* QUESTION: What r to choose s.t. cryptographically still ok? My thoughts are that if mr is too large the interval that z
is chosen from is too small which breaks the purpose of a crypto rng
Also obviously mr must be < 2^k - 1, s.t. there exists a z satisfying the condition n = z*mr+1 for the first n. 
*/
int dirichlet_generate_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n0, int r){
    // create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

    // init n0 to 1
    BN_set_word(n0, (BN_ULONG) 1);
    BIGNUM *bn_prime;
    bn_prime = BN_new();

    // compute product of first r-1 odd primes
    for(int i=1; i<r; i++){
        BN_set_word(bn_prime, (BN_ULONG) primes[i]);
        if(!BN_mul(n0, n0, bn_prime, ctx)){
            return 0;
        }
    }

    BN_CTX_free(ctx);
    BN_free(bn_prime);

    return 1;
}
