#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>
#include "openssl_pga.h"

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int t;
    int l = 0;

	int r = atoi(argv[1]); // get r from program argument
    int seq_nr = atoi(argv[2]); // get seq nr from program argument

	char filename[256];
	sprintf(filename, "data/distribution/openssl_benchmark/openssl_sieve/1strun_r%d_nr%d.csv", r, seq_nr);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"byte\n");

    //retrieve optimal params
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

    BN_set_word(p, 42);
    unsigned char * buffer = malloc((BN_num_bytes(p)) * sizeof(unsigned char));
    int len = BN_bn2bin(p, buffer);
    printf("length: %d", len);

    for(int i=0; i<10; i++){

        //int returncode = openssl_pga(p, k, t, r, l, openssl_generate_sieve, openssl_sieve);
    }

	BN_free(p);
	fclose(fd);

	return 0;
}