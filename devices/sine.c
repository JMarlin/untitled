#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#define DEG_TO_RAD(x) (((x)*M_PI)/180.0)

typedef struct FixedSineGeneratorEnv {
    float current_phase; //Position along the t axis in degrees
    float freq;          //Frequency of the generator
    float duration;      //How long to generate the signal in ms
    float time;           //How long the signal has been running in ms
} FixedSineGeneratorEnv;

int fixed_sine_generator(float* sample, void* environment) {

    FixedSineGeneratorEnv* vars = (FixedSineGeneratorEnv*)environment;

    float deg_per_sample = (360.0 * vars->freq)/SAMPLE_RATE;

    vars->time += 1000.0/SAMPLE_RATE;

    if(vars->time >= vars->duration) 
        return 0;

    vars->current_phase += deg_per_sample;

    if(vars->current_phase >= 360.0)
        vars->current_phase -= 360.0;

    sample[0] = sinf(DEG_TO_RAD(vars->current_phase));

    return 1;
}

SignalSourceMono_f* new_fixed_sine(float freq, float duration) {

    SignalSourceMono_f* signal = new_ssmf(fixed_sine_generator);
    
    if(!signal)
        return signal;

    FixedSineGeneratorEnv* environment = (FixedSineGeneratorEnv*)malloc(sizeof(FixedSineGeneratorEnv));

    if(!environment) {
    
        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->current_phase = 90.0;
    environment->freq = freq;
    environment->duration = duration;
    environment->time = 0;

    signal->environment = environment;

    return signal;
}

typedef struct SineVcoGeneratorEnv {
    float current_phase;
    unsigned last_state;
    ControlVoltage* control_voltage;
} SineVcoGeneratorEnv;

int sine_vco_generator(float* sample, void* environment) {
    
    SineVcoGeneratorEnv* vars = (SineVcoGeneratorEnv*)environment;
    float cv_pitch_sample, cv_gate_sample;

    //If the control signal drops, we drop
    if(!cv_pull_next_sample(vars->control_voltage, &cv_pitch_sample, &cv_gate_sample)) {
        
        sample[0] = 0;
        return 0;
    }
    
    //Control voltages assume a volt-to-hertz scheme covering eight
    //octaves from A0 to A8  mapped over -1.0 to 1.0
    //(eg: -1.0 = 55hz, 0.0 = 880hz, 1.0 = 14080hz)
    if(cv_gate_sample > -1.0) { 

        if(!vars->last_state) {

            vars->current_phase = 90.0;
            vars->last_state = 1;
        }

        float freq = 55.0 * pow(2, 8.0*(1.0 + cv_pitch_sample)/2.0);
        float deg_per_sample = (360.0 * freq)/SAMPLE_RATE;

        sample[0] = sinf(DEG_TO_RAD(vars->current_phase)); 

        vars->current_phase += deg_per_sample;

        if(vars->current_phase >= 360.0)
            vars->current_phase -= 360.0;
    } else {
   
        sample[0] = 0;
        vars->last_state = 0;
    }

    return 1;
}

SignalSourceMono_f* new_sine_vco(ControlVoltage* control_voltage) {

    SignalSourceMono_f* signal = new_ssmf(sine_vco_generator);

    if(!signal)
        return signal;

    SineVcoGeneratorEnv* environment = (SineVcoGeneratorEnv*)malloc(sizeof(SineVcoGeneratorEnv));

    if(!environment) {

        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->current_phase = 90.0;
    environment->last_state = 0;
    environment->control_voltage = control_voltage;
    
    signal->environment = environment;

    return signal;
}

