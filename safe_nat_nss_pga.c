#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "safe_nat_nss_pga.h"

/* 
function: generates prime with natural algorithm, passed on into bignum p
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, r = number of primes to do trial division with, l = max deviation from output prime to trial 
returns: 1 if successful, 0 if failure, -1 if error
*/

int safe_nat_nss_pga(BIGNUM *p, int k, int t, int r, int l){
    
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

    // initialize bn that holds right shifted value for testing safe prime generation
    BIGNUM *rs;
	rs = BN_new();

    unsigned short *sieve;
    int sieve_sz = l/2;
    sieve = (unsigned short*) malloc(sizeof(short)*(sieve_sz)); 

    if(!BN_rand(n0, k, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD)){
		ret = -1;
        goto free_bn_sieve;
    }

    again:
        if(!safe_nat_nss_sieve_init(sieve, sieve_sz, n0, r)){
            ret = -1;
            goto free_bn_sieve;
        }
        
        unsigned long it = 0; // is passed on as an iterator variable inside openssl_sieve to do the sieve checking. 

        ret = safe_nat_nss_sieve(sieve, sieve_sz, n, n0, r, &it, k);

        // check for bit length of returned n
        if(BN_num_bits(n) != k){
            ret = 0;
            goto free_bn_sieve;
        }

        if(ret != 1){ //if we overrun the sieve, we tried l/2 candidates, so retry again with "sliding window"
            goto again;
        }

        if(ret == 1){
            if(!BN_rshift1(rs, n)){
                ret = -1;
                goto free_bn_sieve;
            }
        }

        // pre-condition: n is odd, k-bit long
        while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL) || !BN_is_prime_fasttest_ex(rs, t, ctx, 0, NULL)){
            
            //int nat_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it)
            ret = safe_nat_nss_sieve(sieve, sieve_sz, n, n0, r, &it, k);
            
            // check for bit length of returned n
            if(BN_num_bits(n) != k){
                ret = 0;
                goto free_bn_sieve;
            }

            if(ret != 1){ //if we overrun the sieve, we tried l/2 candidates, so retry again with "sliding window"
                goto again;
            }

            // get rs = n>>1 to check safe prime generation (rs = (n-1)/2, -1 omitted as all primes >2 are odd)
            if(!BN_rshift1(rs, n)){
                ret = -1;
                goto free_bn_sieve;
            }
        }
        // post-condition: n and (n-1)/2 passed t MR-rounds, so probable prime 

        if(!BN_copy(p, n)){ // copy value from n to p on successful prime generation
            ret = -1;
            goto free_bn_sieve;
        } 

    free_bn_sieve:
        free(sieve);
    free_bn:
        BN_free(n0);
        BN_free(n);
        BN_free(rs);
        BN_CTX_free(ctx);
    
    return ret;
}

int safe_nat_nss_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k){
	int ret = 0;

    int overrun = 0;

    BIGNUM *add_bn;
    add_bn = BN_new();

	// if we overrun sieve array, we want to update n0 for reinit
	if(*it >= sieve_sz){
        overrun = 1;
		goto skip;
	}

	// iterate until next zero entry of sieve
	unsigned long deref = *it;
	while(deref < sieve_sz && sieve[deref] != 0){
		deref++;

		if(deref >= sieve_sz){
            overrun = 1;
			goto skip;
		}
	}

    skip:
    if(overrun){
        (*it) = (*it) + 2;
        unsigned long add_value = 4 * (*it);

        if(!BN_set_word(add_bn, add_value)){
            ret = -1;
            goto free_bn;
        }

        // place result into n0 for reinit
        if(!BN_add(n0, n0, add_bn)){
            ret = -1;
            goto free_bn;
        }
        ret = 0;
        //n0 is now updated for new run
    }else{
        *it = deref;
        (*it)++;

        // Precondition: sieve[*it] = 0 and *it < sieve_sz
        // n = n0+4*(*it) as we know that it passed our sieve
        unsigned long add_value = 4 * deref;

        if(!BN_set_word(add_bn, add_value)){
            ret = -1;
            goto free_bn;
        }

        // place result into n
        if(!BN_add(n, n0, add_bn)){
            ret = -1;
            goto free_bn;
        }
        ret = 1;
        // Postcondition: n now contains n0+2*it, which did not fail our trial division sieve
    }

	free_bn:
		BN_free(add_bn);

	return ret;
}

int safe_nat_nss_sieve_init(unsigned short *sieve, int sieve_sz, BIGNUM *n0, int r){
    // set bit at position 1 is set to 1, as else (n0+it)/2 will be even (so not safe prime)
    if(!BN_set_bit(n0, 1)){
        return -1;
    }

	int ret = 0;
	unsigned long offset;

	memset(sieve, 0, sieve_sz*sizeof(*sieve)); // init sieve values all to 0

	// internal buffer for computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// used to store current prime as BN
	BIGNUM *idx_prime;
	idx_prime = BN_new();

	// used to store remainder of n0 mod prime
	BIGNUM *rem;
	rem = BN_new();

	for(int i=1; i<r; i++){
		// fetch current prime from primes table
		unsigned long current_prime = primes[i];
		
		if(!BN_set_word(idx_prime, current_prime)){
			ret = -1;
			goto free_bn;
		}

		if(!BN_mod(rem, n0, idx_prime, ctx)){
			ret = -1;
			goto free_bn;
		}

		if(BN_is_zero(rem)){
			offset = 0;
		}else{
			offset = current_prime - BN_get_word(rem); // n0 + (prime[i] - rem) divible by prime[i]
		}

        /*
         sieve[i] being non-zero means that n0+4*i or ((n0-1)+4*i)/2 is composite. 
         This is because offset calculates the nearest divisible number by prime[i]
         and sets all consecuent additions of prime[i] as divisible as well.
        */
		for(int idx = offset; idx < 4 * sieve_sz; idx += current_prime){
			if( idx % 4 == 0){
				sieve[idx/4] = 1;
			}
		}
		
		// to not skip first occurence
		if(offset == current_prime-1){
			offset = -1;
		}
		for(int idx = offset+1; idx < 4 * sieve_sz; idx += current_prime){
			if( idx % 4 == 0){
				sieve[idx/4] = 1;
			}
		}
		

	}

	ret = 1;

	free_bn:
		BN_CTX_free(ctx);
		BN_free(idx_prime);
		BN_free(rem);

	return ret;
}
