#include "../channel/channel.h"
#include "../config.h"
#include "adsr.h"
#include <stdlib.h>
#include "controlvoltage.h"
//#include "vco.h"

typedef struct ADSRGeneratorEnv_s {
    ControlVoltage* control_voltage;
    SignalSourceMono_f* input_signal;
    SignalSourceMono_f* a_signal;
    SignalSourceMono_f* d_signal;
    SignalSourceMono_f* s_signal;
    SignalSourceMono_f* r_signal;
    float time;
    float last_gate;
} ADSRGeneratorEnv;

int adsr_deleter(void* environment) {

    //ssmf_delete(vars->gate_signal);
    //ssmf_delete(vars->input_signal);
    //ssmf_delete(vars->a_signal);
    //ssmf_delete(vars->d_signal);
    //ssmf_delete(vars->s_signal);
    //ssmf_delete(vars->r_signal);
    return 1;
}

int adsr_generator(float* sample, void* environment) {

    ADSRGeneratorEnv* vars = (ADSRGeneratorEnv*)environment;

    float current_gain;
    float pitch_sample, gate_sample,  input_sample,  a_sample,  d_sample,  s_sample,  r_sample;
    int   cv_running, input_running, a_running, d_running, s_running, r_running;
    
    cv_running  = cv_pull_next_sample(vars->control_voltage,  &pitch_sample, &gate_sample);
    input_running = ssmf_pull_next_sample(vars->input_signal, &input_sample);
    a_running     = ssmf_pull_next_sample(vars->a_signal,     &a_sample);
    d_running     = ssmf_pull_next_sample(vars->d_signal,     &d_sample);
    s_running     = ssmf_pull_next_sample(vars->s_signal,     &s_sample);
    r_running     = ssmf_pull_next_sample(vars->r_signal,     &r_sample);

    //Exit early if any of our input signals exited
    if(!(cv_running && input_running && a_running && d_running && s_running && r_running))
        return 0;

    if(gate_sample > -1.0) {

        //Check to see if we just turned on
        if(!(vars->last_gate > -1.0))
            vars->time = 0;

        if(vars->time <= a_sample) {

            current_gain = ((2*vars->time)/a_sample) - 1;
            vars->time += MS_PER_SAMPLE;
        } else if(vars->time <= (a_sample + d_sample))  {

            current_gain = (((s_sample - 1)*(vars->time - a_sample))/d_sample) + 1;
            vars->time += MS_PER_SAMPLE;
        } else {

            current_gain = s_sample;
            //Don't increase time because it doesn't matter anymore
        }
             

    } else {
  
        //Check to see if we just turned off
        if(vars->last_gate > -1.0)
            vars->time = 0;

        if(vars->time <= r_sample) {
        
            current_gain = (((-1.0 - s_sample) * vars->time)/r_sample)+s_sample;
            vars->time += MS_PER_SAMPLE;
        } else {
        
            current_gain = -1.0;
            //Don't increase time because it doesn't matter anymore
        }
    }

    vars->last_gate = gate_sample;
    current_gain = (current_gain + 1.0)/2.0;
    sample[0] = input_sample * current_gain;

    return 1;
}

SignalSourceMono_f* new_adsr(SignalSourceMono_f* input_signal, ControlVoltage* control_voltage, SignalSourceMono_f* a_signal, SignalSourceMono_f* d_signal, SignalSourceMono_f* s_signal, SignalSourceMono_f* r_signal) {

    ADSRGeneratorEnv* environment = (ADSRGeneratorEnv*)malloc(sizeof(ADSRGeneratorEnv*));

    if(!environment)
        return (SignalSourceMono_f*)0;

    environment->input_signal = input_signal;
    environment->control_voltage = control_voltage;
    environment->a_signal = a_signal;
    environment->d_signal = d_signal;
    environment->s_signal = s_signal;
    environment->r_signal = r_signal;
    environment->time = 0;

    SignalSourceMono_f* out_signal = new_ssmf(adsr_generator); //adsr_deleter, environment);

    if(!out_signal) {

        adsr_deleter(environment); //|
        free(environment);         //'--- Won't need either of these as the above will auto-free the environment on fail
        return out_signal;
    }

    //Delete
    out_signal->environment = environment;

    return out_signal;
}

