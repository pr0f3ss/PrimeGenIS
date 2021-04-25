#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "openssl_pga.h"
#include "nss_pga.h"

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int t;
	int l = 0; // max deviation

	int r = atoi(argv[1]); // get r from program argument

	int l_inc = 512;

	char filename[256];
	sprintf(filename, "data/openssl_benchmark/openssl_sieve/2ndrun_r%d.csv", r);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

	while(l<1){
		FILE *fd_params = fopen("data/optimal_params/openssl_pga/openssl_k1024_r16_r8080.csv");
		int curr_r = 0;
		int curr_t;

		while(curr_r != r){
			fscanf(fd_params, "%d,%d", &curr_r, &curr_t);
		}

		t = curr_t;

		clock_t start, end;
		double cpu_time_used;

		start = clock();

		for(int i=0; i<8192; i++){
			int returncode = openssl_pga(p, k, t, r, l, openssl_generate_sieve, openssl_sieve);
		}

		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		cpu_time_used /= 8192.;

		fprintf(fd, "%d, %d, %f\n", r, l, cpu_time_used);

		if(l>0x4000){
			l_inc = 1024;
		}

		l += l_inc;
	}	
	
	BN_free(p);
	fclose(fd);

	return 0;
}