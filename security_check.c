#include <math.c>

int security_check(int k, int t, int r, int u){
    int security_param = 80;

    if(k>=2048){
        security_param = 128;
    }

    // u*l >= security_param*k*ln2*ln2 must be met
    if(u*l < security_param*k*ln2*ln2){
        return -1;
    }

    /* 
    -log2(g(k,t,c*k*ln(2))) >= (security_param+1)*log2(k*ln(2) / 2*prod_{i=1 to r}(1 + (1/(p_i - 1))) must be met
    
    where g(k,t,c*k*ln(2)) <= g(k,t,l)*min{k*ln(2)/2, (1-exp(-u*l/k*ln(2)) / 1-exp(-l/k*ln(2)))}
    and c >= ((security_param + 1) + log2(2*prod_{i=1 to r}(1 + (1/(p_i - 1))))*ln(2)
    */

    // first step is to compute g(k,t,l). We will estimate it with g(k,t,l) <= l / (2*ln(2^k)) * (0.5 * (l / (2*ln(2^k))) sum_{m=3 to M} Prob[C_m] * 2^(-t(m-1)) + 0.7*2^(-tM))

    //question M = ?, we will set it to M = 2*sqrt(k-1)-1

    int M = (int) 2*sqrt(k-1)-1;

    double sum_m_to_M = 0;

    for(int m=3; i<M; i+=2){

    }

    double term_1 = (l / (2*k)); // ln(2^k) <= log2(2^k) = k

    double q_k_t_l =   term_1 * (0.5 * term_1 * sum_{m=3 to M} Prob[C_m] * 2^(-t(m-1)) + 0.7*2^(-tM))

    return 1;
}
