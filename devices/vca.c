#include "../channel/channel.h"
#include "../util.h"

typedef struct VCAGeneratorEnv {
    SignalSourceMono_f* input_signal;
    SignalSourceMono_f* v_signal;
} VCAGeneratorEnv;

int vca_deleter(void* environment) {

   //vars
   //ssmf_delete(vars->input_signal);
   //ssmf_delete(vars->v_signal);

   return 1; 
}

int vca_generator(float* sample, void* environment) {

    VCAGeneratorEnv* vars = (VCAGeneratorEnv*)environment;

    float input_sample, v_sample;
    int input_running = ssmf_pull_next_sample(vars->input_signal, &input_sample);
    int v_running = ssmf_pull_next_sample(vars->v_signal, &v_sample);

    if(!(input_running && v_running)) {
        
        sample[0] = 0;
        return 0;
    }

    v_sample = (v_sample + 1) / 2;
    sample[0] = input_sample * v_sample;

    return 1;
}

SignalSourceMono_f* new_vca(SignalSourceMono_f* input_signal, SignalSourceMono_f* v_signal) {

    VCAGeneratorEnv* environment = new(VCAGeneratorEnv);

    if(!environment) 
        return (SignalSourceMono_f*)0;

    environment->input_signal = input_signal;
    environment->v_signal = v_signal;

    SignalSourceMono_f* out_signal = new_ssmf(vca_generator); //vca_deleter, environment);

    //Delete
    if(out_signal)
        out_signal->environment = environment;
    
    return out_signal;
}
