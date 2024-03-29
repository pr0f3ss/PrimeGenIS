#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <openssl/bn.h>

int main(int argc, char **argv){

	BIGNUM *p;
	p = BN_new();

	// seen as set
	int k = 1024; //bitsize
	int t = 5; // MR rounds
	int l = 0; // max deviation

	int r = 0;

	int l_inc = 512;

	char filename[256];
	sprintf(filename, "data/openssl_benchmark/native_implementation/saferun_r%d.csv", r);
	FILE *fd;
	fd = fopen(filename, "w+");
	fprintf(fd,"r, l, avgruntime\n");

	while(l<1){
		
		clock_t start, end;
		double cpu_time_used;

		start = clock();

		for(int i=0; i<8192; i++){
			int returncode = BN_generate_prime_ex(p, 1024, 1, NULL, NULL, NULL);
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