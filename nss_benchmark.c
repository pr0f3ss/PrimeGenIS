#include <stdio.h>
#include <time.h>
#include <openssl/bn.h>
#include "nss_pga.h"

int main(){

	BIGNUM *p;
	p = BN_new();

	BN_CTX *ctx;
	ctx = BN_CTX_new();

	// seen as set
	int k = 1024; //bitsize
	int u = 10; // max nss_iter rounds
	int t = 5;

	int r_arr[5] = {512,1024,2048,4096,8192}; // number of primes
	int l_arr[5] = {128, 1024, 10000, 0x8000, 0xF000}; // 0x8000 = 2^15

	FILE *fd;
	fd = fopen("nss_analysis.csv", "w+");
	fprintf(fd,"r, l, avgruntime\n");

	for(int r=0; r<5; r++){
		for(int l=0; l<5; l++){

			time_t begin, end;
    		time(&begin);

			for(int i=0; i<512; i++){
				//int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);
				int returncode = nss_pga(p, k, t, u, r_arr[r], l_arr[l]);
			}

			time(&end);
    		double diff = difftime(end, begin);

			printf("Test done for: r=%d, l=%d in %fs\n", r_arr[r],l_arr[l],diff);
			fprintf(fd, "%d, %d, %f\n", r_arr[r],l_arr[l],diff);

		}
	}
	

	BN_CTX_free(ctx);
	BN_free(p);
	fclose(fd);

	return 0;
}