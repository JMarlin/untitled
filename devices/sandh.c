#include "../channel/channel.h"
#include "../config.h"
#include "../util.h"
#include <stdlib.h>

typedef struct SAndHGeneratorEnv_s {
    float last_sample;
    SignalSourceMono_f* input_signal;
    SignalSourceMono_f* trigger_signal;
} SAndHGeneratorEnv;

int sandh_generator(float* sample, void* environment) {

    float trigger_sample, input_sample;

    SAndHGeneratorEnv* vars = (SAndHGeneratorEnv*)environment;
    int input_running = ssmf_pull_next_sample(vars->input_signal, &input_sample);
    int trigger_running = ssmf_pull_next_sample(vars->trigger_signal, &trigger_sample);

    if(!(input_running && trigger_running))
        return 0;

    if(trigger_sample >= 1.0)
        vars->last_sample = input_sample;
    
    sample[0] = vars->last_sample;

    return 1;
}

SignalSourceMono_f* new_sandh(SignalSourceMono_f* input_signal, SignalSourceMono_f* trigger_signal) {

    SAndHGeneratorEnv* environment = new(SAndHGeneratorEnv);

    if(!environment) 
        return (SignalSourceMono_f*)0;
    
    environment->input_signal = input_signal;
    environment->trigger_signal = trigger_signal;
    environment->last_sample = 0;
    
    SignalSourceMono_f* out_signal = new_ssmf(sandh_generator); //, sandh_deleter, environment);

    //Delete later
    out_signal->environment = environment;

    return out_signal;
}

