#include <openssl/bn.h>
#include <openssl/rand.h>

int main(){
	/* ======================== GENERATE RANDOM BIGNUM AND TEST FOR PRIMALITY ========================*/

	// used for library functions to store BIGNUMs
	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// our prime
	BIGNUM *p;
	p = BN_new();

	// openssl auto-seeds itself with RAND_poll
	BN_rand(p, 1024, 0, 1);


	BN_GENCB *cb;
	cb = BN_GENCB_new();

	// test for primality
	if(BN_check_prime(p, ctx, NULL)){
		printf("Random number: success\n");
	}else{
		printf("Random number: fail\n");
	}


	/*============================= GENERATE PRIME ===============================================*/
	BIGNUM *r;
	r = BN_new();

	if(BN_generate_prime_ex(r, 1024, 0, NULL, NULL, NULL)){
		printf("Generate: success\n");
	}else{
		printf("Generate: fail\n");
	}


	// free all datastructs
	BN_CTX_free(ctx);
	BN_free(p);
	BN_GENCB_free(cb);

	BN_free(r);


	return 0;
}