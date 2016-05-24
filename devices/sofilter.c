#include "../channel/channel.h"
#include "../config.h"
#include "sofilter.h"
#include <stdlib.h>

typedef struct SOFilterGeneratorEnv {
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
    float ta;
    float tb;
    SignalSourceMono_f* input_signal;
} SOFilterGeneratorEnv;

int so_filter_generator(float* sample, void* environment) {

    SOFilterGeneratorEnv* vars = (SOFilterGeneratorEnv*)environment;
    float input_sample;
    
    if(!ssmf_pull_next_sample(vars->input_signal, &input_sample)) {
        
        sample[0] = 0;
        return 0;
    }
    
    float ta_incoming = input_sample - (vars->a2 * vars->tb) - (vars->a1 * vars->ta);
    sample[0] = (vars->b0 * ta_incoming) + (vars->b2 * vars->tb) + (vars->b1 * vars->ta);
    vars->tb = vars->ta;
    vars->ta = ta_incoming;

    return 1;
}

SignalSourceMono_f* new_so_filter(SignalSourceMono_f* input_signal, float b0, float b1, float b2, float a1, float a2) {

    SignalSourceMono_f* signal = new_ssmf(so_filter_generator);
    
    if(!signal)
        return signal;

    SOFilterGeneratorEnv* environment = (SOFilterGeneratorEnv*)malloc(sizeof(SOFilterGeneratorEnv));

    if(!environment) {
    
        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->b0 = b0;
    environment->b1 = b1;
    environment->b2 = b2;
    environment->a1 = a1;
    environment->a2 = a2;
    environment->ta = 0;
    environment->tb = 0;
    environment->input_signal = input_signal;

    signal->environment = environment;

    return signal;
}
