#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "safe_nss_pga.h"
#include "safe_openssl_pga.h"
#include "safe_nat_pga.h"
#include "safe_dirichlet_sieve.h"

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int u = 1000; // max nss_iter rounds
	int t = 8; // MR rounds

	int r = atoi(argv[1]);
	int seq = atoi(argv[2]);
    int l = atoi(argv[3]); // max deviation

	char filename[256];
	sprintf(filename, "data/nss_benchmark/nss_sieve/saferun_r%d_l%d_nr%d.csv", r, l, seq);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    for(int i=0; i<1; i++){
        int returncode = safe_nss_pga(p, k, t, u, r, l, safe_nss_generate_sieve, safe_nss_sieve);
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cpu_time_used /= 1.;

    fprintf(fd, "%d, %d, %f\n", r, l, cpu_time_used);
	
	BN_free(p);
	fclose(fd);

	return 0;
}