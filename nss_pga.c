#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "nss_pga.h"

/* 
function: generates prime with nss method, passed on into bignum p
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, u = # max nss_iter rounds, r = number of primes to do trial division with, l = max deviation from output prime to trial 
returns: 1 if successful, 0 if failure, -1 if error
*/

int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l, int (*generate_sieve)(unsigned char*, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int)){

	if(k<=0 || u<=0){
		printf("k and u must be >0");
		return -1;
	}

	if(r <= 0 || 8192 < r){
		printf("r must lie between 0 and 8192");
		return -1;
	}

	BIGNUM *n;
	n = BN_new();

	int ret = -1; // return code of nss_iter
	int j = 0; // iteration var

	// tries at most u nss_iter steps
	while(ret != 1 && j < u){
		ret = nss_iter(n, k, r, t, l, generate_sieve, sieve_algo);
		j+=1;
	}

	if(ret == 1){
		BN_copy(p, n); // copy value from n to p on successful prime generation
	}

	BN_free(n);

	return ret;
}


/*
function: represents one step of nss method
arguments: p = returned prime if successfully generated, k = bit size of prime, r = number of primes to do trial division with, t = # MR rounds, l = max deviation from output prime to trial
returns: 1 if successful, 0 if failure, -1 if error (sieve generation)
other:  l = max deviation from initially generated num and probable prime 
*/

int nss_iter(BIGNUM *p, int k, int r, int t, int l, int (*generate_sieve)(unsigned char*, int, BIGNUM*, int), int (*sieve_algo)(unsigned char*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int)){

	// create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// generate k-bit, odd number, aka trial value
	BIGNUM *n0;
	n0 = BN_new();
	BN_rand(n0, k, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD); // question: Do top bits have to be 1 for valid k bit prime output?

	// p is going to copy from this var on success
	BIGNUM *n;
	n = BN_new();

	// rem represents the remainder, used as an exit condition if n deviates too much from n0 (captured in l)
	BIGNUM *rem;
	rem = BN_new();

	int ret = -1; // return code of nss_sieve



	/* ========= SIEVE GENERATION SECTION ============= */

	unsigned char *sieve;
	int sieve_sz = l/2;

	sieve = (unsigned char*) malloc(sieve_sz); 

	// generate sieve for nss_sieve method
	int returncode_sievegen = generate_sieve(sieve, sieve_sz, n0, r);
	if(returncode_sievegen != 1){

		// todo: figure out how to skip do while loop if sieve generation fails (to free all buffers correctly) without having to repeat code
		BN_free(n);
		BN_free(rem);
		BN_CTX_free(ctx);
		free(sieve);

		return ret;
	}

	/* ========= END SIEVE GENERATION SECTION ============= */

	// holds l in a bignum
	BIGNUM *bn_l;
	bn_l = BN_new();

	BN_set_word(bn_l, l);


	unsigned long it = 0; // is passed on as an iterator variable inside nss_sieve to do the sieve checking. 

	do{
		ret = sieve_algo(sieve, sieve_sz, n, n0, r, &it, k);
		
		BN_sub(rem, n, n0);

		// question: Is l a 'free' parameter or do we consider it as set? What value should it hold? (Maybe n0/2?)
		if(ret != 1 || !BN_cmp(rem, bn_l) || BN_num_bits(n) != k){
			ret = 0;

			BN_free(n);
			BN_free(rem);
			BN_free(bn_l);
			BN_CTX_free(ctx);
			free(sieve);

			return ret;

		}

	}while(!BN_is_prime_fasttest_ex(n, t, ctx, 0, NULL)); // signature: int BN_is_prime_fasttest_ex(const BIGNUM *p, int nchecks, BN_CTX *ctx, int do_trial_division, BN_GENCB *cb);


	if(ret == 1){
		BN_copy(p, n); // copy value from n to p on successful prime generation
	}

	BN_free(n);
	BN_free(rem);
	BN_free(bn_l);
	BN_CTX_free(ctx);
	free(sieve);

	return ret;
}


/*
function: implements the nss sieve, if sieve[i] is non-zero then (n0+2*i) is composite. We traverse the sieve until we find entry i, s.t. sieve[i] = 0. This means that n0+2*i did not fail in our sieve
and did not get detected as a composite yet. We set n = n0+2*i and return a success. In the case we go over sieve_sz, we return failure.
arguments: ieve = passed on datastructure holding the sieve values, sieve_sz = size of the sieve, n = probable prime if successful, n0 = initial sieve number aka trial, r = number of primes to do trial division with
returns: 1 if successful, 0 if failure, -1 if error 
*/

int nss_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n, BIGNUM *n0, int r, unsigned long *it, int k){
	// if we overrun sieve array, return failure
	if(*it >= sieve_sz){
		return 0;
	}	
	// iterate until next zero entry of sieve
	unsigned long deref = *it;
	//printf("before: %lu, ", deref);
	while(deref < sieve_sz && sieve[deref] != 0){
		deref++;

		if(deref >= sieve_sz){
			return 0;
		}
	}

	*it = deref;
	(*it)++;

	//printf("after: %lu\n", deref);
	// Precondition: sieve[*it] = 0 and *it < sieve_sz
	// n = n0+2*(*it) as we know that it passed our sieve
	unsigned long add_value = 2 * deref;

	BIGNUM *add_bn;
	add_bn = BN_new();

	BN_set_word(add_bn, add_value);

	// place result into n
	BN_add(n, n0, add_bn);

	// Postcondition: n now contains n0+2*it, which did not fail our trial division sieve

	free(add_bn);

	//printf("returned value = 1\n");

	return 1;
}

/*
function: fills sieve according to trial n0 for usage in nss_sieve
arguments: sieve = passed on datastructure holding the sieve values, sieve_sz = size of the sieve, n0 = initial sieve number aka trial, r = number of primes to do trial division with 
returns: 1 if successful, 0 if error 
*/

int nss_generate_sieve(unsigned char *sieve, int sieve_sz, BIGNUM *n0, int r){
	int ret = 1;
	unsigned long offset;

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	memset(sieve, 0, sieve_sz); // init sieve values all to 0

	// used to store current prime as BN
	BIGNUM *idx_prime;
	idx_prime = BN_new();

	// used to store remainder of n0 mod prime
	BIGNUM *rem;
	rem = BN_new();

	for(int i=0; i<r; i++){
		// fetch current prime from primes table (containing the first 2048 primes)
		unsigned long current_prime = primes[i];
		
		BN_set_word(idx_prime, current_prime);

		if(!BN_mod(rem, n0, idx_prime, ctx)){
			ret = 0;
		}

		if(BN_is_zero(rem)){
			offset = 0;
		}else{
			offset = current_prime - BN_get_word(rem);
		}

		for(int idx = offset; idx < 2 * sieve_sz; idx += current_prime){
			if( idx % 2 == 0){
				sieve[idx/2] = 1;
			}
		}
	}

	BN_CTX_free(ctx);
	BN_free(idx_prime);
	BN_free(rem);

	return ret;

}