#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "nss_pga.h"
#include "dirichlet_sieve.h"

int main(){
	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int u = 10; // max nss_iter rounds
	int t = 5; // MR rounds

	int l = 4000; // max deviation

	int r = 33; 

	int l_inc = 512;

	FILE *fd;
	fd = fopen("data/dirichlet_benchmark/dirichlet_nss_analysis_r33.csv", "w+");
	fprintf(fd,"r, l, avgruntime\n");

	while(l<0x10000){
		
		clock_t start, end;
		double cpu_time_used;

		start = clock();

		for(int i=0; i<8192; i++){
			//int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);
			int returncode = nss_pga(p, k, t, u, r, l, dirichlet_generate_sieve, dirichlet_sieve);
		}

		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		cpu_time_used /= 8192.;

		fprintf(stdout, "Test done for: r=%d, l=%d in %fs\n", r, l, cpu_time_used);
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