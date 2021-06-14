#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "safe_nss_pga.h"

/* 
function: generates safe prime with nss method, passed on into bignum p
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, u = # max nss_iter rounds, r = number of primes to do trial division with, l = max deviation from output prime to trial 
returns: 1 if successful, 0 if failure, -1 if error
*/

int safe_nss_pga(BIGNUM *p, int k, int t, int u, int r, int l, int (*generate_sieve)(unsigned short*, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int), int sieve_sz){
	
	BIGNUM *n;
	n = BN_new();

	int ret = -1; // return code of nss_iter
	int j = 0; // iteration var

	unsigned short *sieve = NULL;
    sieve = (unsigned short*) malloc(sizeof(short)*(sieve_sz)); 

	// tries at most u safe_nss_iter steps
	while(ret != 1 && j < u){
		ret = safe_nss_iter(n, k, r, t, l, generate_sieve, sieve_algo, sieve);
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
function: represents one step of nss method
arguments: p = returned prime if successfully generated, k = bit size of prime, r = number of primes to do trial division with, t = # MR rounds, l = max deviation from output prime to trial
returns: 1 if successful, 0 if failure, -1 if error (sieve generation)
other:  l = max deviation from initially generated num and probable prime 
*/

int safe_nss_iter(BIGNUM *p, int k, int r, int t, int l, int (*generate_sieve)(unsigned short*, int, BIGNUM*, int), int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int), unsigned short* sieve){
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

    // initialize bn that holds right shifted value for testing safe prime generation
    BIGNUM *rs;
	rs = BN_new();

	// rem represents the remainder, used as an exit condition if n deviates too much from n0 (captured in l)
	BIGNUM *rem;
	rem = BN_new();

	// holds l in a bignum
	BIGNUM *bn_l;
	bn_l = BN_new();

	if(!BN_rand(n0, k, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD)){
		ret = -1;
		goto free_bn;
	}

	/* ========= SIEVE GENERATION SECTION ============= */

	int sieve_sz = l/2;

	// generate sieve for nss_sieve method
	if(!generate_sieve(sieve, sieve_sz, n0, r)){
		ret = -1;
		goto free_bn;
	}

	/* ========= END SIEVE GENERATION SECTION ============= */

	if(!BN_set_word(bn_l, l)){
		ret = -1;
		goto free_bn;
	}

	unsigned long it = 0; // is passed on as an iterator variable inside nss_sieve to do the sieve checking. 

	// pre-condition: n0 holds trial value
	do{
		ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);
		
		BN_sub(rem, n, n0);

		if(ret != 1 || !BN_cmp(rem, bn_l) || BN_num_bits(n) != k){
			ret = 0;
			goto free_bn;
		}

        // get rs = n>>1 to check safe prime generation (rs = (n-1)/2, -1 omitted as all primes >2 are odd)
        if(!BN_rshift1(rs, n)){
            ret = -1;
            goto free_bn;
        }

	}while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL) || !BN_is_prime_fasttest_ex(rs, t, ctx, 0, NULL)); // signature: int BN_is_prime_fasttest_ex(const BIGNUM *p, int nchecks, BN_CTX *ctx, int do_trial_division, BN_GENCB *cb);

	// post-condition: ret = 1, n passed t MR-rounds and n-n0 < l

	if(!BN_copy(p, n)){ // copy value from n to p on successful prime generation
		ret = -1;
	} 

	free_bn:
		BN_free(n);
		BN_free(n0);
        BN_free(rs);
		BN_free(rem);
		BN_free(bn_l);
		BN_CTX_free(ctx);

	return ret;
}


/*
function: implements the nss sieve, if sieve[i] is non-zero then (n0+2*i) or ((n0-1)+2*i) is composite. We traverse the sieve until we find entry i, s.t. sieve[i] = 0. This means that n0+2*i did not fail in our sieve
and did not get detected as a composite yet. We set n = n0+2*i and return a success. In the case we go over sieve_sz, we return failure.
arguments: ieve = passed on datastructure holding the sieve values, sieve_sz = size of the sieve, n = probable prime if successful, n0 = initial sieve number aka trial, r = number of primes to do trial division with
returns: 1 if successful, 0 if failure, -1 if error 
*/

int safe_nss_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k){
	int ret = 0;

	// if we overrun sieve array, return failure
	if(*it >= sieve_sz){
		return ret;
	}	
	// iterate until next zero entry of sieve
	unsigned long deref = *it;
	while(deref < sieve_sz && sieve[deref] != 0){
		deref++;

		if(deref >= sieve_sz){
			return ret;
		}
	}

	*it = deref;
	(*it)++;

	// Precondition: sieve[*it] = 0 and *it < sieve_sz
	// n = n0+4*(*it) as we know that it passed our sieve
	unsigned long add_value = 4 * deref;

	BIGNUM *add_bn;
	add_bn = BN_new();

	if(!BN_set_word(add_bn, add_value)){
		ret = -1;
		goto free_bn;
	}

	// place result into n
	if(!BN_add(n, n0, add_bn)){
		ret = -1;
		goto free_bn;
	}

	// Postcondition: n now contains n0+2*it, which did not fail our trial division sieve

	ret = 1;

	free_bn:
		BN_free(add_bn);

	return ret;
}

/*
function: fills sieve according to trial n0 for usage in nss_sieve
arguments: sieve = passed on datastructure holding the sieve values, sieve_sz = size of the sieve, n0 = initial sieve number aka trial, r = number of primes to do trial division with 
returns: 1 if successful, 0 if error 
*/

int safe_nss_generate_sieve(unsigned short *sieve, int sieve_sz, BIGNUM *n0, int r){
    // set bit at position 1 is set to 1, as else (n0+it)/2 will be even (so not safe prime)
    if(!BN_set_bit(n0, 1)){
        return -1;
    }

	int ret = 0;
	unsigned long offset;

	memset(sieve, 0, sizeof(short)*sieve_sz); // init sieve values all to 0

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