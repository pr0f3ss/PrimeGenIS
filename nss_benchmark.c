#include <stdio.h>
#include <time.h>
#include <openssl/bn.h>
#include "nss_pga.h"

int main(){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int u = 10; // max nss_iter rounds
	int t = 5;

	int r = 1024;
	int l = 4000; 

	int r_inc = 16;
	int l_inc = 512;

	FILE *fd;
	fd = fopen("nss_analysis.csv", "w+");
	fprintf(fd,"r, l, avgruntime\n");

	while(r<8192){

		l = 4000;
		l_inc = 512;

		while(l<0x20000){
			
			clock_t start, end;
			double cpu_time_used;

			start = clock();

			for(int i=0; i<8192; i++){
				//int nss_pga(BIGNUM *p, int k, int t, int u, int r, int l);
				int returncode = nss_pga(p, k, t, u, r, l);
			}

			end = clock();
    		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

			printf("Test done for: r=%d, l=%d in %fs\n", r, l, cpu_time_used);
			fprintf(fd, "%d, %d, %f\n", r, l, cpu_time_used);

			if(l>0x4000){
				l_inc = 1024;
			}

			l += l_inc;
		}

		if(r>3000){
			r_inc = 64;
		}

		r += r_inc;
	}
	
	BN_free(p);
	fclose(fd);

	return 0;
}