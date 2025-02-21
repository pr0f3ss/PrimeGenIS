#include <openssl/bn.h>
#include "safe_nat_pga.h"
#include "safe_dirichlet_sieve.h"

int main(){

	BIGNUM *p;
	p = BN_new();

	BIGNUM *s;
	s = BN_new();

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	int l = 0; // 0x8000 = 2^15 = 32768
	int t = 8; 

	for(int i=0; i<100; i++){
		int returncode = safe_nat_pga(p, 1024, t, 32, l, safe_dirichlet_generate_sieve, safe_dirichlet_sieve);
		printf("returncode: %d\n", returncode);

		BN_rshift1(s, p);
      
		// test for primality
		if(!BN_check_prime(p, ctx, NULL) || !BN_check_prime(s, ctx, NULL) ){
			printf("[!] Fail\n");
			return -1;
		}
	}

	BN_CTX_free(ctx);
	BN_free(p);
	BN_free(s);

	printf("Generated primes successfully without error!\n");

	return 0;
}

