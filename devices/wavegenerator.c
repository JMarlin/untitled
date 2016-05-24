#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "wavegenerator.h"
#include <stdlib.h>
#include <math.h>

typedef struct FixedWaveGeneratorEnv {
    float current_phase; //Position along the t axis in degrees
    float freq;          //Frequency of the generator
    float duration;      //How long to generate the signal in ms
    float time;           //How long the signal has been running in ms
    WaveFunction wave_function;
    void* wave_environment;
} FixedWaveGeneratorEnv;

int fixed_wave_generator(float* sample, void* environment) {

    FixedWaveGeneratorEnv* vars = (FixedWaveGeneratorEnv*)environment;

    float deg_per_sample = (360.0 * vars->freq)/SAMPLE_RATE;

    vars->time += 1000.0/SAMPLE_RATE;

    if(vars->time >= vars->duration) 
        return 0;

    vars->current_phase += deg_per_sample;

    if(vars->current_phase >= 360.0)
        vars->current_phase -= 360.0;

    sample[0] = vars->wave_function(vars->current_phase, vars->wave_environment);

    return 1;
}

SignalSourceMono_f* new_fixed_wave(float freq, float duration, WaveFunction wave_function, void* wave_environment) {

    SignalSourceMono_f* signal = new_ssmf(fixed_wave_generator);
    
    if(!signal)
        return signal;

    FixedWaveGeneratorEnv* environment = (FixedWaveGeneratorEnv*)malloc(sizeof(FixedWaveGeneratorEnv));

    if(!environment) {
    
        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->current_phase = 90.0;
    environment->freq = freq;
    environment->duration = duration;
    environment->wave_function = wave_function;
    environment->wave_environment = wave_environment;
    environment->time = 0;

    signal->environment = environment;

    return signal;
}

typedef struct VcoWaveGeneratorEnv {
    float current_phase;
    unsigned last_state;
    WaveFunction wave_function;
    void* wave_environment;
    ControlVoltage* control_voltage;
} VcoWaveGeneratorEnv;

int vco_wave_generator(float* sample, void* environment) {
    
    VcoWaveGeneratorEnv* vars = (VcoWaveGeneratorEnv*)environment;
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

        sample[0] = vars->wave_function(vars->current_phase, vars->wave_environment); 

        vars->current_phase += deg_per_sample;

        if(vars->current_phase >= 360.0)
            vars->current_phase -= 360.0;
    } else {
   
        sample[0] = 0;
        vars->last_state = 0;
    }

    return 1;
}

SignalSourceMono_f* new_vco_wave(ControlVoltage* control_voltage, WaveFunction wave_function, void* wave_environment) {

    SignalSourceMono_f* signal = new_ssmf(vco_wave_generator);

    if(!signal)
        return signal;

    VcoWaveGeneratorEnv* environment = (VcoWaveGeneratorEnv*)malloc(sizeof(VcoWaveGeneratorEnv));

    if(!environment) {

        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->current_phase = 90.0;
    environment->last_state = 0;
    environment->wave_function = wave_function;
    environment->wave_environment = wave_environment;
    environment->control_voltage = control_voltage;
    
    signal->environment = environment;

    return signal;
}

