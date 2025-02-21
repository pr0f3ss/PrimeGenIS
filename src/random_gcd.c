#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "dirichlet_sieve.h"

int main(){
    int ret = 0;
    int r = 33;

    FILE *fd;
	fd = fopen("benchmark/data/coprime_gen/random.csv", "w+");
	fprintf(fd,"r, avgruntime\n");

    // ===================== INIT VARS ======================

    // create buffer for internal computations
    BN_CTX *ctx;
    ctx = BN_CTX_new();

    // initialize mr with sieve
    BIGNUM *n0;
    n0 = BN_new();

    unsigned char *sieve;
	int sieve_sz = 0;

    if(!dirichlet_generate_sieve(&sieve, sieve_sz, n0, r)){
        ret = -1;
        goto free_bn;
    }

    // initialize constant '1'
    BIGNUM *bn_one;
    bn_one = BN_new();
    if(!BN_set_word(bn_one, (BN_ULONG) 1)){ // = 1
        ret = -1;
        goto free_bn;
    }

    // holds gcd value
    BIGNUM *bn_gcd;
    bn_gcd = BN_new();

    // a
    BIGNUM *bn_a;
    bn_a = BN_new();

    // ================= START TEST =======================

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    for(int i=0; i<32000; i++){
        do{
            if(!BN_rand_range(bn_a, n0)){ // generate number in [0, mr]
                ret = -1;
                goto free_bn;
            }
            
            if(!BN_add(bn_a, bn_a, bn_one)){ // bn_a in [1, mr+1]
                ret = -1;
                goto free_bn;
            } 

            if(!BN_gcd(bn_gcd, n0, bn_a, ctx)){ // compute gcd(n0, bn_a)
                ret = -1;
                goto free_bn;
            }
        }while(!BN_is_one(bn_gcd));
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    cpu_time_used /= 32000.;

    fprintf(fd, "%d, %f\n", r, cpu_time_used);

    ret = 1;

    free_bn:
        BN_CTX_free(ctx);
        BN_free(n0);
        BN_free(bn_one);
        BN_free(bn_gcd);
        BN_free(bn_a);
        free(sieve);
        fclose(fd);

    return ret;
}


