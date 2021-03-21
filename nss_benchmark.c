#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "nss_pga.h"

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int u = 10; // max nss_iter rounds
	int t = 5; // MR rounds

	int l = 4000; // max deviation

	int r = atoi(argv[1]); // get r from program argument

	int l_inc = 512;

	char filename[64];
	sprintf(filename, "data/nss_benchmark/nss_analysis_r%d.csv", r);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

	while(l<0x10000){
		
		clock_t start, end;
		double cpu_time_used;

		start = clock();

		for(int i=0; i<8192; i++){
			//int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);
			int returncode = nss_pga(p, k, t, u, r, l);
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