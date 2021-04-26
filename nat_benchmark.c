#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "nat_pga.h"
#include "dirichlet_sieve.h"


int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int t; // MR rounds
	int l = 0; // max deviation

	int r = atoi(argv[1]); // get r from program argument

	int l_inc = 512;

	char filename[256];
	sprintf(filename, "data/nat_benchmark/dirichlet_sieve/2ndrun_r%d.csv", r);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

	FILE *fd_params = fopen("data/optimal_params/nat_pga/nat_dir_k1024_r8_r100.csv", "r");
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

		for(int i=0; i<8192; i++){
			int returncode = nat_pga(p, k, t, r, l, dirichlet_generate_sieve, dirichlet_sieve);
		}

		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		cpu_time_used /= 8192.;

		fprintf(stdout, "Test done for: r=%d, l=%d in %fs\n", r, l, cpu_time_used);
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