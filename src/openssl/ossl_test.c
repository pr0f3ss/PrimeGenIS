#include <openssl/bn.h>
#include "openssl_pga.h"

int main(){

	BIGNUM *p;
	p = BN_new();

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	int l = 0; // 0x8000 = 2^15 = 32768
	int t = 8; 

	for(int i=0; i<1; i++){
		int returncode = safe_openssl_pga(p, 1024, t, 1024, l, openssl_generate_sieve, openssl_sieve);
		//printf("returncode: %d\n", returncode);
		printf("i: %d\n", i);

		// test for primality
		if(!BN_check_prime(p, ctx, NULL)){
			printf("[!] Fail\n");
			return -1;
		}
	}

	BN_CTX_free(ctx);
	BN_free(p);

	printf("Generated primes successfully without error!\n");

	return 0;
}