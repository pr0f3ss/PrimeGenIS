#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include "openssl_pga.h"

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

    BIGNUM *n0;
	n0 = BN_new();

	// create buffer for internal computations
	BN_CTX *ctx;
	ctx = BN_CTX_new();


	// seen as set
	int k = 1024; //bitsize
	int t = 8;
	int l = 0; // max deviation

	int r = atoi(argv[1]); // get r from program argument

	char filename[256];
	sprintf(filename, "data/openssl_benchmark/debug/1strun_r%d.csv", r);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

    clock_t start, end;
    double cpu_time_used;
    RAND_poll();

    start = clock();

    for(int i=0; i<8192; i++){
    	//int iterations = 0;
    	do{
	        if(!BN_rand(n0, k, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD)){
	            goto free_bn;
	        }
	        unsigned char *sieve;
	        int sieve_sz = 0;
	        if(!openssl_generate_sieve(&sieve, sieve_sz, n0, r)){
	            goto free_bn;
	        }
	        unsigned long it = 0; // is passed on as an iterator variable inside openssl_sieve to do the sieve checking. 
	        int ret = openssl_sieve(sieve, sieve_sz, p, n0, r, &it, k);
	        //iterations++;

        }while(!BN_is_prime_fasttest_ex(p, t, ctx, 0, NULL));
        //printf("iterations: %d, bitsize: %d\n", iterations, BN_num_bits(p));

    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cpu_time_used /= 8192.;

    fprintf(fd, "%d, %d, %f\n", r, l, cpu_time_used);
    printf("Done\n");

    free_bn:
        BN_free(p);
        BN_free(n0);
        fclose(fd);
        BN_CTX_free(ctx);

	return 0;
}