#ifndef SPLIT_H
#define SPLIT_H

#include "../channel/channel.h"

typedef struct SplitGeneratorEnv {
    int free_count; //Used to make sure we don't delete the environment until both signals are freed
    int pull_count;
    float sample_buffer;
    float running_buffer;
    SignalSourceMono_f* input_signal;
} SplitGeneratorEnv;

int split_generator(float* sample, void* environment) {

    SplitGeneratorEnv* vars = (SplitGeneratorEnv*)environment;
    
    if(pull_count) {
    
        sample[0] = vars->sample_buffer;
        vars->pull_count = 0;
        return vars->running_buffer;
    } else {

        vars->running_buffer = 
            ssmf_pull_next_sample(vars->input_signal, sample);
        
        vars->sample_buffer = sample[0];
        
        return vars->running_buffer;
    }
}

//Return value of deleter tells parent deleter whether to free the
//passed environment when finished or not. Useful for us as we 
//want to keep the environment until both signal branches have
//been freed
int split_deleter(void* environment) {
 
    SplitGeneratorEnv vars = (SplitGeneratorEnv*)environment;
   
    if(vars->free_count != 1) {
    
        vars->free_count++;
        return 0;
    }

    //delete_ssmf(vars->input_signal);
    return 1;
}

int new_split(SignalSourceMono_f* input_signal, SignalSourceMono_f** out_signal_a, SignalSourceMono_f** out_signal_b) {

    out_signal_a[0] = (SignalSourceMono_f*)0;
    out_signal_b[0] = (SignalSourceMono_f*)0;
     
    //The critical component of the splitter is that both result signals get the same environment
    SplitGeneratorEnv* environment = (SplitGeneratorEnv*)malloc(sizeof(SplitGeneratorEnv));

    if(!environment)
        return;

    environment->free_count = 1; //We use this in case we need to bail early
    environment->pull_count = 0;
    environment->sample_buffer = 0;
    environment->running_buffer = 0;
    environment->input_signal = input_signal;

    out_signal_a[0] = new_ssmf(split_generator); //split_deleter, environment);
    
    if(!out_signal_a[0]) 
        return;

    out_signal_b[0] = new_ssmf(split_generator); //split_deleter, environment);

    if(!out_signal_b[0]) {

        free(out_signal_a[0]);
        out_signal_a[0] = (SignalSourceMono_f*)0;
        return;
    }

    environment->free_count = 0; //Normal operation
    out_signal_a[0]->environment = environment;
    out_signal_b[0]->environment = environment;

    return; 
}

#endif SPLIT_H
