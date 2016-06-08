#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "wavegenerator.h"
#include <stdlib.h>
#include <math.h>

typedef struct FixedWaveGeneratorEnv_s {
    float current_phase; //Position along the t axis in degrees
    float freq;          //Frequency of the generator
    WaveFunction wave_function;
    void* wave_environment;
} FixedWaveGeneratorEnv;

int fixed_wave_generator(float* sample, void* environment) {

    FixedWaveGeneratorEnv* vars = (FixedWaveGeneratorEnv*)environment;

    float deg_per_sample = (360.0 * vars->freq)/SAMPLE_RATE;

    vars->current_phase += deg_per_sample;

    if(vars->current_phase >= 360.0)
        vars->current_phase -= 360.0;

    sample[0] = vars->wave_function(vars->current_phase, vars->wave_environment);

    return 1;
}

SignalSourceMono_f* new_fixed_wave(float freq, WaveFunction wave_function, void* wave_environment) {

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
    environment->wave_function = wave_function;
    environment->wave_environment = wave_environment;

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
    float deg_per_sample = (360.0 * cv_pitch_sample)/SAMPLE_RATE;

    sample[0] = vars->wave_function(vars->current_phase, vars->wave_environment); 

    vars->current_phase += deg_per_sample;

    if(vars->current_phase >= 360.0)
        vars->current_phase -= 360.0;

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

