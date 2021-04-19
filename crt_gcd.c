#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "dirichlet_sieve.h"

int gcdExtended(BIGNUM* a, BIGNUM* b, BIGNUM* ri, BIGNUM* si, BIGNUM* gcd){

    // create buffer for internal computations
    BN_CTX *ctx;
    ctx = BN_CTX_new();
    
    BIGNUM *bn_r1;
    bn_r1 = BN_new();

    BIGNUM *bn_s1;
    bn_s1 = BN_new();

    BIGNUM *bn_divid;
    bn_divid = BN_new();

    // base case
    if(BN_is_zero(a)){
        if(!BN_set_word(ri, (BN_ULONG) 0)){ // = 0
            ret = -1;
            goto free_bn;
        }
        if(!BN_set_word(si, (BN_ULONG) 1)){ // = 1
            ret = -1;
            goto free_bn;
        }
        if(!BN_copy(gcd, b)){ // = b
            ret = -1;
            goto free_bn;
        }
        
        ret = 1;
        goto free_bn;
    }

    if(!BN_mod(b, b, a, ctx)){ // compute b%a
        ret = -1;
        goto free_bn;
    }

    gcdExtended(b, a, r1, s1, gcd);

    if(!BN_copy(si, r1)){ // si = r1
        ret = -1;
        goto free_bn;
    }

    if(!BN_div(bn_divid, NULL, b, a, ctx)){ // = b/a
        ret = -1;
        goto free_bn;
    }

    if(!BN_mul(bn_divid, bn_divid, r1)){ // b/a * r1
        ret = -1;
        goto free_bn;
    }

    if(!BN_sub(s1, s1, bn_divid)){ // = s1 - (b/a)*r1
        ret = -1;
        goto free_bn;
    }

    if(!BN_copy(ri, s1)){ // ri = s1 - (b/a)*r1
        ret = -1;
        goto free_bn;
    }

    ret = 1;
    free_bn:
        BN_CTX_free(ctx);
        BN_free(r1);
        BN_free(s1);
        BN_free(bn_divid);

    return ret;
}

int main(){
    int ret = 0;
    int r = 33;

    FILE *fd;
	fd = fopen("benchmark/data/coprime_gen/crt.csv", "w+");
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

    if(!generate_sieve(&sieve, sieve_sz, n0, r)){
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

    // init constant '2'
    BIGNUM *bn_two;
    bn_two = BN_new();
    if(!BN_set_word(bn_two, (BN_ULONG) 2)){ // = 2
        ret = -1;
        goto free_bn;
    }

    // holds gcd value
    BIGNUM *bn_gcd;
    bn_gcd = BN_new();

    // a
    BIGNUM *bn_a;
    bn_a = BN_new();

    // pi
    BIGNUM *bn_pi;
    bn_pi = BN_new();

    // bignum arr
    BIGNUM* arr_ai[r];
    for(int i=0; i<r; i++){
        arr_i[i] = BN_new();
    }

    BIGNUM* bn_MI;
    bn_MI = BN_new();

    BIGNUM* bn_ri;
    bn_ri = BN_new();

    BIGNUM* bn_si;
    bn_si = BN_new();


    // ================= START TEST =======================

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    for(int i=0; i<32000; i++){

        if(!BN_set_word(bn_a, (BN_ULONG) 0)){ // reset
            ret = -1;
            goto free_bn;
        }

        for(int j=0; j<r; j++){
            if(!BN_set_word(bn_pi, (BN_ULONG) primes[j])){ // = pi
                ret = -1;
                goto free_bn;
            }

            if(!BN_sub(bn_pi, bn_pi, bn_two)){ // = pi-2
                ret = -1;
                goto free_bn;
            } 

            if(!BN_rand_range(arr_ai[j], bn_pi)){ // generate number in [0, pi-2]
                ret = -1;
                goto free_bn;
            }

            if(!BN_add(arr_ai[j], arr_ai[j], bn_one)){ // put ai in [1, pi-1]
                ret = -1;
                goto free_bn;
            }

            if(!BN_div(bn_MI, NULL, n0, bn_pi, ctx)){ // MI = mr / pi
                ret = -1;
                goto free_bn;
            }

            gcdExtended(bn_pi, bn_MI, bn_ri, bn_si, bn_gcd); //we need to get si, algo changes inputs so we have to retrieve them again

            if(!BN_set_word(bn_pi, (BN_ULONG) primes[j])){ // = pi
                ret = -1;
                goto free_bn;
            }

            if(!BN_div(bn_MI, NULL, n0, bn_pi, ctx)){ // MI = mr / pi
                ret = -1;
                goto free_bn;
            }

            if(!BN_mul(bn_si, bn_si, bn_MI)){ // ei = si * MI
                ret = -1;
                goto free_bn;
            }

            if(!BN_mul(bn_ri, arr_ai[j], bn_si)){ // = ai*ei
                ret = -1;
                goto free_bn;
            }

            if(!BN_add(bn_a, bn_a, bn_ri)){ // a = sum ai*ei 
                ret = -1;
                goto free_bn;
            }
        }

        if(!BN_gcd(bn_gcd, bn_a, n0, ctx)){
            ret = -1;
            goto free_bn;
        }

        if(BN_is_one(bn_gcd)){
            printf("Good iteration: %d\n", i);
        }
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
        BN_free(bn_two);
        BN_free(bn_gcd);
        BN_free(bn_a);
        BN_free(bn_pi);
        BN_free(bn_MI);
        BN_free(bn_ri);
        BN_free(bn_si);
        free(sieve);
        fclose(fd);
        for(int i=0; i<r; i++){
            BN_free(arr_ai[i]);
        }

    return ret;
}


