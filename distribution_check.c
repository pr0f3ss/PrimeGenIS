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
	sprintf(filename, "data/distribution/openssl/openssl_sieve/1strun_r%d_nr%d.csv", r, seq_nr);
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
		//printf("%d\n", curr_r);
	}
	
	t = curr_t;
	fclose(fd_params);

	char binseq[8];
	unsigned char * buffer = malloc(128 * sizeof(unsigned char)); //k=1024
	for(int i=0; i<32000; i++){

        	int returncode = openssl_pga(p, k, t, r, l, openssl_generate_sieve, openssl_sieve);
	
   		int len = BN_bn2bin(p, buffer);
		unsigned char last_byte = buffer[0];
		int binary[8];
		for(int n=0; n<8; n++){
			binary[7-n] = (last_byte >> n) & 1;
		}
		
		for(int n=0; n<8; n++){
			binseq[n] =  binary[n]+'0';
		}
		fprintf(fd, "%s\n", binseq);
	}

	BN_free(p);
	fclose(fd);
	free(buffer);
	return 0;
}
