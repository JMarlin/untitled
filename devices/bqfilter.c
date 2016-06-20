#include "../channel/channel.h"
#include "../config.h"
#include "bqfilter.h"
#include <stdlib.h>

typedef struct BQFilterGeneratorEnv_s {
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
    float d1;
    float d2;
    SignalSourceMono_f* input_signal;
} BQFilterGeneratorEnv;

int bq_filter_generator(float* sample, void* environment) {

    BQFilterGeneratorEnv* vars = (BQFilterGeneratorEnv*)environment;
    float input_sample;
    
    if(!ssmf_pull_next_sample(vars->input_signal, &input_sample)) {
        
        sample[0] = 0;
        return 0;
    }
    
    sample[0] = (vars->b0 * input_sample) + (vars->b1 * vars->d1) + (vars->b2 * vars->d2) - (vars->a1 * vars->d1) - (vars->a2 * vars->d2);
    vars->d2 = vars->d1;
    vars->d1 = sample[0];

    return 1;
}

SignalSourceMono_f* new_bq_filter(SignalSourceMono_f* input_signal, float b0, float b1, float b2, float a1, float a2) {

    SignalSourceMono_f* signal = new_ssmf(bq_filter_generator);
    
    if(!signal)
        return signal;

    BQFilterGeneratorEnv* environment = (BQFilterGeneratorEnv*)malloc(sizeof(BQFilterGeneratorEnv));

    if(!environment) {
    
        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->b0 = b0;
    environment->b1 = b1;
    environment->b2 = b2;
    environment->a1 = a1;
    environment->a2 = a2;
    environment->d1 = 0;
    environment->d2 = 0;
    environment->input_signal = input_signal;

    signal->environment = environment;

    return signal;
}
