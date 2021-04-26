#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "nss_pga.h"
#include "openssl_pga.h"

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int u; // max nss_iter rounds
	int t; // MR rounds

	int l = 700; // max deviation

	int r = atoi(argv[1]);

	int l_inc = 512;

	char filename[256];
	sprintf(filename, "data/nss_benchmark/openssl_sieve/2ndrun_r%d.csv", r);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

	while(l<=65536){
		char opt_filename[256];
		sprintf(opt_filename, "data/optimal_params/nss_pga/nss_k1024_r%d_l700_l65536.csv", r);
		FILE *fd_params = fopen(opt_filename, "r");
		int curr_r = 0;
		int curr_l = 0;
		int curr_t;
		int curr_u;
		int ret;
		
		fscanf(fd_params, "%*[^\n]\n"); //skip header
		while((ret = fscanf(fd_params, "%d, %d, %d, %d", &curr_r, &curr_l, &curr_u, &curr_t) != EOF)){
			if(curr_r == r && curr_l == l){
				break;
			}		
		}
		t = curr_t;
		u = curr_u;

		fclose(fd_params);
		
		clock_t start, end;
		double cpu_time_used;

		start = clock();

		for(int i=0; i<8192; i++){
			int returncode = nss_pga(p, k, t, u, r, l, openssl_generate_sieve, openssl_sieve);
		}

		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		cpu_time_used /= 8192.;

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