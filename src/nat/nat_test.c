#include <openssl/bn.h>
#include "nat_pga.h"
#include "dirichlet_sieve.h"
#include "nss_pga.h"

int main(){

	BIGNUM *p;
	p = BN_new();

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	int l = 0x8000; // 0x8000 = 2^15 = 32768
	int t = 5; 

	for(int i=0; i<1000; i++){
		//int nat_pga(BIGNUM *p, int k, int t, int r, int l);
		int returncode = nat_pga(p, 2048, t, 200, l, dirichlet_generate_sieve, dirichlet_sieve);
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