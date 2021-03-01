#include <openssl/bn.h>
#include <openssl/rand.h>
#include "primes.h"

int main(){

	unsigned long prime = primes[24];

	BIGNUM *a;
	a = BN_new();

	BN_set_word(a, prime);

	if(BN_is_word(a, prime)){
		printf("Success: %d bits length\n", BN_num_bits(a));
	}

	BN_free(a);

	return 0;
}
