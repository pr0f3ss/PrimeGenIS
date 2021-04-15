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

arguments: sieve = not used, sieve_sz = not used, n {returned if success} = z*mr+1 or n=n+mr (when it!=0) , n0 = mr (product of first r odd primes), r = number of primes to do trial division with, it = iterator variable, k = bitsize
returns: 1 if successful, 0 if failure, -1 if error 
*/

int dirichlet_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k){
    int ret = 0;

    if(*it == 0){

        // create buffer for internal computations
        BN_CTX *ctx;
        ctx = BN_CTX_new();

        // initialize constant '1'
        BIGNUM *bn_one;
        bn_one = BN_new();

        // initialize a
        BIGNUM *bn_a;
        bn_a = BN_new();

        // holds gcd value
        BIGNUM *bn_gcd;
        bn_gcd = BN_new();

        // initialize lower interval bound = 2^(k-1) + 1, will first be initialized as 2^(k-1)
        BIGNUM *bn_lw;
        bn_lw = BN_new();

        // initialize upper interval bound = 2^k
        BIGNUM *bn_up;
        bn_up = BN_new();

        // initialize value for shifting rng interval = 2^(k-1) - a
        BIGNUM *bn_shift_interval;
        bn_shift_interval = BN_new();

        // initialize bn holding remainder
        BIGNUM *rem;
        rem = BN_new();
        
        if(!BN_set_word(bn_one, (BN_ULONG) 1)){ // = 1
            ret = -1;
            goto free_bn;
        }

        // this do-while loop generates 'a' relatively prime to mr (gcd(mr,a) = 1)
        do{
            if(!BN_rand_range(bn_a, n0)){ // generate number in [0, mr]
                ret = -1;
                goto free_bn;
            }
            
            if(!BN_add(bn_a, bn_a, bn_one)){ // bn_a in [1, mr+1]
                ret = -1;
                goto free_bn;
            } 

            if(!BN_gcd(bn_gcd, n0, bn_a, ctx)){ // compute gcd(n0, bn_a)
                ret = -1;
                goto free_bn;
            }
        }while(!BN_is_one(bn_gcd));

        //post-cond: a is relatively prime to mr

        // set the values
        if(!BN_set_word(bn_lw, (BN_ULONG) 0)){ // = 0
            ret = -1;
            goto free_bn;
        }
        if(!BN_set_bit(bn_lw, (k-1))){ // = 2^(k-1)
            ret = -1;
            goto free_bn;
        }

        if(!BN_sub(bn_shift_interval, bn_lw, bn_a)){ // = 2^(k-1) - a
            ret = -1;
            goto free_bn;
        } 

        if(!BN_add(bn_lw, bn_lw, bn_one)){  // = 2^(k-1) + 1
            ret = -1;
            goto free_bn;
        }

        
        if(!BN_set_word(bn_up, (BN_ULONG) 0)){ // = 0
            ret = -1;
            goto free_bn;
        }
        if(!BN_set_bit(bn_up, k)){ // = 2^k
            ret = -1;
            goto free_bn;
        }

        if(!BN_sub(bn_up, bn_up, bn_lw)){ // = (2^k - (2^(k-1) + 1))
            ret = -1;
            goto free_bn;
        } 

        if(!BN_rand_range(n, bn_up)){ // generate number in [0, (2^k - (2^(k-1) + 1)]
            ret = -1;
            goto free_bn;
        }

        // precondition: n is out of interval [0, (2^k - (2^(k-1) + 1)]

        if(!BN_add(n, n, bn_shift_interval)){ // shift the interval from [0, (2^k - (2^(k-1) + 1)] to [2^(k-1) - a, 2^k - (a+1)]
            ret = -1;
            goto free_bn;
        }

        if(!BN_mod(rem, n, n0, ctx)){ // get remainder of n/mr, s.t. we have a z for which holds: n = z*mr
            ret = -1;
            goto free_bn;
        }

        if(!BN_sub(n, n, rem)){ // subtract the remainder, s.t. n divisible by mr
            ret = -1;
            goto free_bn;
        }

        // postcondition: n is out of interval [2^(k-1) - a, 2^k - (a+1)] AND n is divisible by mr which implies n = z*mr for some z

        if(!BN_add(n, n, bn_a)){ // construct n = z*mr + a
            ret = -1;
            goto free_bn;
        }

        *it = 1; // set it=1 to signal that next iteration should just add mr
        ret = 1;

        // free all bignums used
        free_bn:
            BN_free(bn_one);
            BN_free(bn_a);
            BN_free(bn_gcd);
            BN_free(bn_lw);
            BN_free(bn_up);
            BN_free(bn_shift_interval);
            BN_free(rem);
            BN_CTX_free(ctx);
    }else{
        ret = 1;

        if(!BN_add(n, n, n0)){ // n = n+mr
            ret -1;
        }
    }

    return ret;
}

/*
function: calculates mr for usage in the dirichlet sieve
arguments: sieve = not used, sieve_sz = not used, n0 = mr if successful, r = number of primes to do trial division with 
returns: 1 if successful, 0 if failure, -1 if error 
*/
int dirichlet_generate_sieve(unsigned char **sieve, int sieve_sz, BIGNUM *n0, int r){
    int ret = 0;
    
    // allocate 1 byte s.t. we can free in any pga without issues
    *sieve = NULL;
    *sieve = (unsigned char*) malloc(1); 

    if(*sieve == NULL){
        return -1;
    } 

    // create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

    // init n0 to 1
    BIGNUM *bn_prime;
    bn_prime = BN_new();

    if(!BN_set_word(n0, (BN_ULONG) 1)){
        ret = -1;
        goto free_bn;
    }
    
    // compute product of first r-1 odd primes
    for(int i=1; i<r; i++){
        if(!BN_set_word(bn_prime, (BN_ULONG) primes[i])){
            ret = -1;
            goto free_bn;
        }

        if(!BN_mul(n0, n0, bn_prime, ctx)){
            ret = -1;
            goto free_bn;
        }
    }

    ret = 1;

    free_bn:
        BN_CTX_free(ctx);
        BN_free(bn_prime);

    return ret;
}
