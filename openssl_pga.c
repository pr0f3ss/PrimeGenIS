#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "openssl_pga.h"

/* 
function: generates prime with openssl algorithm, passed on into bignum p
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, r = number of primes to do trial division with, l = max deviation from output prime to trial 
returns: 1 if successful, 0 if general error
*/
int openssl_pga(BIGNUM *p, int k, int t, int r, int l, int (*generate_sieve)(unsigned char**, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int)){
    int ret = -1;
    
    BIGNUM *n;
	n = BN_new();

    while(ret==-1){
        ret = openssl_iter(n, k, r, t, l, generate_sieve, sieve_algo);
    }

    if(ret == 1){
		BN_copy(p, n); // copy value from n to p on successful prime generation
	}

    BN_free(n);

    return ret;

}

/*
function: represents one step of openssl method, basically initializes n0 and its sieve and then tries to retrieve n from sieve method. Checks the returned n in t MR-rounds
arguments: p = returned prime if successfully generated, k = bit size of prime, r = number of primes to do trial division with, t = # MR rounds, l = max deviation from output prime to trial
returns: 1 if successful, 0 if sieve generation failed, -1 if failure in creating prime 
other:  l = max deviation from initially generated num and probable prime 
*/
int openssl_iter(BIGNUM *p, int k, int r, int t, int l, int (*generate_sieve)(unsigned char**, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int)){

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

    /* ========= SIEVE GENERATION SECTION ============= */
    unsigned char *sieve;
	int sieve_sz = l/2;

    if(!generate_sieve(&sieve, sieve_sz, n0, r)){
        BN_free(n0);
        BN_free(n);
        BN_CTX_free(ctx);
        free(sieve);

        return ret;
    }
    /* ========= END SIEVE GENERATION SECTION ============= */

    unsigned long it = 0; // is passed on as an iterator variable inside openssl_sieve to do the sieve checking. 

	ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);
    
    // check return value of openssl_sieve & for bit length of returned n
    if(ret != 1 || BN_num_bits(n) != k){
        BN_free(n0);
        BN_free(n);
        BN_CTX_free(ctx);
        free(sieve);

        return ret;
    }

    if(BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL)){
        BN_copy(p, n); // copy value from n to p on successful prime generation
    }else{
        ret = -1; // set ret = -1 if didn't pass MR test
    }

    BN_free(n0);
    BN_free(n);
    BN_CTX_free(ctx);
    free(sieve);

    return ret;
}

/*
function: implements the openssl sieve, if sieve[i]+it mod primes[i] == 0 then we conclude that n0+it is composite. We therefore generate trial n0 and fill the sieve according to n0.
We then loop over all primes and check whether the previously mentioned expression (sieve[i]+it mod primes[i] == 0) equates to true. If that is the case, we must increment 'it' by 2 
and start the loop over, now with the updated it variable.
If n0+it overruns l (max deviation), then we retry by running 'retry' again. 
arguments: sieve = passed on datastructure holding the sieve values, sieve_sz = size of the sieve, n = probable prime if successful, n0 = initial sieve number aka trial, r = number of primes to do trial division with
returns: 1 if successful, 0 if general error , -1 if failure in generating probable prime
*/
int openssl_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k){
    loop:
        // check if n0+it passes sieve 
        for(int i=0; i<r; i++){
            if(((unsigned long) sieve[i] + (*it)) % primes[i]  == 0){
                *it = (*it) + 2;

                // if 'it' goes over max deviation value 'l' then retry with another trial n0, remember that l = sieve_sz/2
                if(*it >= 2*sieve_sz){
                    return -1;
                }

                // If sieve[i]+it mod prime[i] == 0, then we conclude that it must be composite, therefore do loop again with updated 'it' variable.
                goto loop;
            }
        }
    

    // n0+it is now a probable prime, copy n0+it into n
    BIGNUM *add_bn;
	add_bn = BN_new();
    unsigned long add_value = *it;

	if(!BN_set_word(add_bn, (BN_ULONG) add_value)){
        BN_free(add_bn);
        return 0;
    }

    if(!BN_add_word(n0, add_bn)){
        BN_free(add_bn);
        return 0;
    }

    if(!BN_copy(n, n0)){
        BN_free(add_bn);
    	return 0;
    }

    BN_free(add_bn);

    return 1;
}

/*
function: fills sieve according to trial n0 for usage in openssl_sieve
arguments: sieve = passed on datastructure holding the sieve values, sieve_sz = size of the sieve, n0 = initial sieve number aka trial, r = number of primes to do trial division with 
returns: 1 if successful, 0 if general error 
*/

int openssl_generate_sieve(unsigned char **sieve, int sieve_sz, BIGNUM *n0, int r){
    *sieve = NULL;
    *sieve = (unsigned char*) malloc(r); 

    if(*sieve == NULL){
        return 1;
    }

    for(int i=0; i<r; i++){
        BN_ULONG mod = BN_mod_word(n0, (BN_ULONG)primes[i]);
        if(mod == (BN_ULONG) -1){
            return 0;
        }
        (*sieve)[i] = (unsigned char) mod;
    }

    return 1;
}
