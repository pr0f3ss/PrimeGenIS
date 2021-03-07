#include <openssl/bn.h>
#include "nss_pga.h"

int main(){

	BIGNUM *p;
	p = BN_new();

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	int l = 0x8000; // 0x8000 = 2^15

	for(int i=0; i<1000; i++){
		//int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);
		int returncode = nss_pga(p, 1024, 5, 10, 2000, l);

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