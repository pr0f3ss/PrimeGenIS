#include <openssl/bn.h>
#include "nss_pga.h"
#include "openssl_pga.h"


int main(){

	BIGNUM *p;
	p = BN_new();

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// security param at k=1024 => g = 80, u*l >= g*ln^2(2)*k ~ 39360
	int l = 0x8000; // 0x8000 = 2^15 = 32768
	int u = 2;
	int t = 5; 

	for(int i=0; i<1000000; i++){
		//int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);
		int returncode = nss_pga(p, 1024, t, u, 1024, l, openssl_generate_sieve, openssl_sieve);

		printf("returncode: %d\n", returncode);

		// test for primality
		if(!BN_check_prime(p, ctx, NULL)){
			printf("[!] Fail\n");
			return -1;
		}

	}

	BN_CTX_free(ctx);
	BN_free(p);

	printf("Generated primes successfully without error!");

	return 0;
}