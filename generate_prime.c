#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include "primes.h"
#include "generate_prime.h"

#include "nat_pga.h"
#include "nss_pga.h"
#include "openssl_pga.h"
#include "dirichlet_sieve.h"

#include "safe_nat_nss_pga.h"
#include "safe_nat_pga.h"
#include "safe_nss_pga.h"
#include "safe_openssl_pga.h"
#include "safe_dirichlet_sieve.h"

/*
Generates prime by using parameters specified by user (used internally)
*/
int generate_prime_spec(BIGNUM *p, int k, int t, int r, int l, int u, int pga_option, int sieve_option, int safe){
    int ret = -1;
    if(safe){
        // special case nat + nss
        if(pga_option == NAT_PGA && sieve_option == NSS_SIEVE){
            ret = safe_nat_nss_pga(p, k, t, r, l);
            return ret;
        }

        int (*generate_sieve)(unsigned short*, int, BIGNUM*, int);
        int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int);
        int sieve_sz;

        if(sieve_option == OSSL_SIEVE){
            generate_sieve = safe_openssl_generate_sieve;
            sieve_algo = safe_openssl_sieve;
            sieve_sz = r-1;
        }else if(sieve_option == NSS_SIEVE){
            generate_sieve = safe_nss_generate_sieve;
            sieve_algo = safe_nss_sieve;
            sieve_sz = l/2;
        }else if(sieve_option == DIR_SIEVE){
            generate_sieve = safe_dirichlet_generate_sieve;
            sieve_algo = safe_dirichlet_sieve;
            sieve_sz = 1;
        }else{
            return ret;
        }

        if(pga_option == OSSL_PGA){
            ret = safe_openssl_pga(p, k, t, r, l, generate_sieve, sieve_algo, sieve_sz);
        }else if(pga_option == NSS_PGA){
            ret = safe_nss_pga(p, k, t, u, r, l, generate_sieve, sieve_algo, sieve_sz);
        }else if(pga_option == NAT_PGA){
            ret = safe_nat_pga(p, k, t, r, l, generate_sieve, sieve_algo, sieve_sz);
        }else{
            return ret;
        }

    }else{
        int (*generate_sieve)(unsigned short*, int, BIGNUM*, int);
        int (*sieve_algo)(unsigned short*, int, BIGNUM*, BIGNUM*, int, unsigned long*, int);
        int sieve_sz;

        if(sieve_option == OSSL_SIEVE){
            generate_sieve = openssl_generate_sieve;
            sieve_algo = openssl_sieve;
            sieve_sz = r-1;
        }else if(sieve_option == NSS_SIEVE){
            generate_sieve = nss_generate_sieve;
            sieve_algo = nss_sieve;
            sieve_sz = l/2;
        }else if(sieve_option == DIR_SIEVE){
            generate_sieve = dirichlet_generate_sieve;
            sieve_algo = dirichlet_sieve;
            sieve_sz = 1;
        }else{
            return ret;
        }

        if(pga_option == OSSL_PGA){
            ret = openssl_pga(p, k, t, r, l, generate_sieve, sieve_algo, sieve_sz);
        }else if(pga_option == NSS_PGA){
            ret = nss_pga(p, k, t, u, r, l, generate_sieve, sieve_algo, sieve_sz);
        }else if(pga_option == NAT_PGA){
            ret = nat_pga(p, k, t, r, l, generate_sieve, sieve_algo, sieve_sz);
        }else{
            return ret;
        }
    }

    return ret;
}