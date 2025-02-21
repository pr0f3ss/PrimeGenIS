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
	int seq = atoi(argv[2]); // get seq nr from program argument

	int l_inc = 512;

	char filename[256];
	sprintf(filename, "data/openssl_benchmark/openssl_sieve/4thrun_r%d_nr%d.csv", r, seq);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

	FILE *fd_params = fopen("data/optimal_params/openssl_pga/openssl_k1024_r16_r8080.csv", "r");
	int curr_r = 0;
	int curr_t;
	int ret;
	fscanf(fd_params, "%*[^\n]\n"); //skip header
	while((ret = fscanf(fd_params, "%d, %d", &curr_r, &curr_t) != EOF) && curr_r != r){			
		printf("%d\n", curr_r);
	}
	
	t = curr_t;
	fclose(fd_params);

	while(l<=1){
		clock_t start, end;
		double cpu_time_used;

		start = clock();

		for(int i=0; i<1; i++){
			int returncode = openssl_pga(p, k, t, r, l, openssl_generate_sieve, openssl_sieve);
		}

		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		cpu_time_used /= 1.;

		fprintf(fd, "%d, %d, %f\n", r, l, cpu_time_used);

		if(l>12000){
			l_inc = 1024;
		}

		l += l_inc;
	}	
	
	BN_free(p);
	fclose(fd);

	return 0;
}