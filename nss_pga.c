#include <openssl/bn.h>
#include <openssl/rand.h>


/* 
function: generates prime with nss method, passed on into bignum p
arguments: p = probable prime if successful, k = bit size of prime, t = # MR rounds, u = # max nss_iter rounds, r = number of primes to do trial division with
returns: 1 if successful, 0 if failure, -1 if error
*/

int nss_pga(BIGNUM *p, int k, int t, int u, int r){

	BIGNUM *n;
	n = BN_new();

	int ret = -1; // return code of nss_iter
	int j = 0; // iteration var

	while(ret != 1 && j < u){
		ret = nss_iter(n, k, r, t);
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
arguments: p = returned prime if successfully generated, k = bit size of prime, r = number of primes to do trial division with, t = # MR rounds
returns: 1 if successful, 0 if failure, -1 if error
other:  l = max deviation from initially generated num and probable prime 
*/

int nss_iter(BIGNUM *p, int k, int r, int t){

	// create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// generate k-bit, odd number, aka trial value
	BIGNUM *n0;
	n0 = BN_new();
	BN_rand(n0, k, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ODD);

	// p is going to be copy from this var on success
	BIGNUM *n;
	n = BN_new();

	// rem represents the remainder, used as an exit condition if n deviates too much from n0 (captured in l)
	BIGNUM *rem;
	rem = BN_new();

	int ret = -1; // return code of nss_sieve

	// generate sieve for nss_sieve method
	if(nss_generate_sieve(sieve, n0, r) != 1){

		// todo: figure out how to skip do while loop if sieve generation fails (to free all buffers correctly)
		ret = 0;
	}

	do{
		ret = nss_sieve(n, n0, r);

		BN_sub(rem, n, n0);

		// TODO: set l
		if(BN_cmp(rem, l) || BN_num_bits(n) != k){
			ret = 0;
		}


	}while(ret != 0 && !bn_is_prime_int(n, t, ctx, 0, NULL)); // signature: bn_is_prime_int(const BIGNUM *w, int checks, BN_CTX *ctx, int do_trial_division, BN_GENCB *cb);



	if(ret == 1){
		BN_copy(p, n); // copy value from n to p on successful prime generation
	}

	BN_free(n);
	BN_free(rem);
	BN_CTX_free(ctx);


	return 1;
}


/*
function: implements the nss sieve
arguments: n = probable prime if successful, n0 = initial sieve number aka trial, r = number of primes to do trial division with
returns: 1 if successful, 0 if failure, -1 if error 
*/

int nss_sieve(BIGNUM *n, BIGNUM *n0, int r){
	
}

/*
function: fills sieve according to trial n0 for usage in nss_sieve
arguments: sieve = passed on datastructure holding the sieve values, n0 = initial sieve number aka trial, r = number of primes to do trial division with 
returns: 1 if successful, 0 if error 
*/

int nss_generate_sieve(unsigned char *sieve, BIGNUM *n0, int r){
	int ret = 1;
	unsigned long offset;

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	memset(sieve, 0, r); // init sieve values all to 0

	BIGNUM *idx_prime;
	idx_prime = BN_new();

	BIGNUM *rem;
	rem = BN_new();

	for(int i=0; i<r; i++){
		/* generate prime table and insert into current_prime
		unsigned long current_prime = primes[i];
		*/

		BN_set_word(idx_prime, current_prime);

		if(ret = BN_mod(rem, n0, idx_prime, ctx) != 1){
			ret = 0;
		}

		if(BN_is_zero(rem)){
			offset = 0;
		}else{
			// TODO: see how to convert remainder to ulong
			offset = current_prime - convert(rem);
		}

		for(int idx = offset; i < 2 * r; i += current_prime){
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