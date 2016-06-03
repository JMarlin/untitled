#include "../channel/channel.h"
#include "controlvoltage.h"
#include "../util.h"
#include <stdlib.h>

typedef struct CVSplitGeneratorEnv_s {
    int free_count; //Used to make sure we don't delete the environment until both signals are freed
    int pull_count;
    float pitch_buffer;
    float gate_buffer;
    float running_buffer;
    ControlVoltage* input_cv;
} CVSplitGeneratorEnv;

int cv_split_generator(float* pitch_sample, float* gate_sample, void* environment) {

    CVSplitGeneratorEnv* vars = (CVSplitGeneratorEnv*)environment;
    
    if(vars->pull_count) {
    
        pitch_sample[0] = vars->pitch_buffer;
        gate_sample[0] = vars->gate_buffer;
        vars->pull_count = 0;
    } else {

        vars->running_buffer = 
            cv_pull_next_sample(vars->input_cv, pitch_sample, gate_sample);
        
        vars->pitch_buffer = pitch_sample[0];
        vars->gate_buffer = gate_sample[0];
        
        vars->pull_count++;
    }

    return vars->running_buffer;
}

//Return value of deleter tells parent deleter whether to free the
//passed environment when finished or not. Useful for us as we 
//want to keep the environment until both signal branches have
//been freed
int cv_split_deleter(void* environment) {
 
    CVSplitGeneratorEnv* vars = (CVSplitGeneratorEnv*)environment;
   
    if(vars->free_count != 1) {
    
        vars->free_count++;
        return 0;
    }

    //delete_ssmf(vars->input_signal);
    return 1;
}

int new_cv_split(ControlVoltage* input_cv, ControlVoltage** out_cv_a, ControlVoltage** out_cv_b) {

    out_cv_a[0] = (ControlVoltage*)0;
    out_cv_b[0] = (ControlVoltage*)0;
     
    //The critical component of the splitter is that both result signals get the same environment
    CVSplitGeneratorEnv* environment = (CVSplitGeneratorEnv*)malloc(sizeof(CVSplitGeneratorEnv));

    if(!environment)
        return;

    environment->free_count = 1; //We use this in case we need to bail early
    environment->pull_count = 0;
    environment->pitch_buffer = 0;
    environment->gate_buffer = 0;
    environment->running_buffer = 0;
    environment->input_cv = input_cv;

    out_cv_a[0] = (ControlVoltage*)malloc(sizeof(ControlVoltage));
    
    if(!out_cv_a[0]) 
        return;

    out_cv_a[0]->generator = cv_split_generator;

    out_cv_b[0] = (ControlVoltage*)malloc(sizeof(ControlVoltage));   

    if(!out_cv_b[0]) {

        free(out_cv_a[0]);
        out_cv_a[0] = (ControlVoltage*)0;
        return;
    }

    out_cv_b[0]->generator = cv_split_generator;

    environment->free_count = 0; //Normal operation
    out_cv_a[0]->environment = environment;
    out_cv_b[0]->environment = environment;

    return; 
}

typedef struct MFSplitGeneratorEnv_s {
    int free_count;
    int pull_count;
    float sample_buffer;
    int running_buffer;
    SignalSourceMono_f* input_signal;
} MFSplitGeneratorEnv;

int mf_split_generator(float* sample, void* environment) {

    MFSplitGeneratorEnv* vars = (MFSplitGeneratorEnv*)environment;

    if(vars->pull_count) {
    
        sample[0] = vars->sample_buffer;
        vars->pull_count = 0;
    } else {

        vars->running_buffer =
            ssmf_pull_next_sample(vars->input_signal, sample);

        vars->sample_buffer = sample[0];
        vars->pull_count++;
    }

    return vars->running_buffer;
}

int mf_split_deleter(void* environment) {

    MFSplitGeneratorEnv* vars = (MFSplitGeneratorEnv*)environment;

    if(vars->free_count != 1) {

        vars->free_count++;
        return 0;
    }

    //delete_ssmf(vars->input_signal);
    return 1;
}

int new_mf_split(SignalSourceMono_f* input_signal, SignalSourceMono_f** out_signal_1, SignalSourceMono_f** out_signal_2) {

    out_signal_1[0] = (SignalSourceMono_f*)0;
    out_signal_2[0] = (SignalSourceMono_f*)0;

    MFSplitGeneratorEnv* environment = new(MFSplitGeneratorEnv);
    
    if(!environment)
        return;

    environment->free_count = 1;
    environment->pull_count = 0;
    environment->sample_buffer = 0;
    environment->running_buffer = 0;
    environment->input_signal = input_signal;
    
    out_signal_1[0] = new(SignalSourceMono_f);

    if(!out_signal_1[0])
         return;

    out_signal_1[0]->generator = mf_split_generator;

    out_signal_2[0] = new(SignalSourceMono_f);

    if(!out_signal_2[0]) {

        free(out_signal_1[0]);
        out_signal_1[0] = (SignalSourceMono_f*)0;
        return;
    }

    out_signal_2[0]->generator = mf_split_generator;

    environment->free_count = 0;
    out_signal_1[0]->environment = out_signal_2[0]->environment = environment;

    return;
}
