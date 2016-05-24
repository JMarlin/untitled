#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "wavegenerator.h"
#include "pulse.h"
#include <math.h>
#include <stdlib.h>

typedef struct PulseVcoEnvironment {
    SignalSourceMono_f* duty_signal;
} PulseVcoEnvironment;

float pulse_wave_function(float phase, void* environment) {
    
    PulseVcoEnvironment* vars = (PulseVcoEnvironment*)environment;
    float duty_sample;

    ssmf_pull_next_sample(vars->duty_signal, &duty_sample);
    duty_sample = (duty_sample + 1.0)/2.0;

    return phase >= (360.0*duty_sample) ? 1.0 : -1.0; 
}

SignalSourceMono_f* new_pulse_vco(ControlVoltage* control_voltage, SignalSourceMono_f* duty_signal) {

    PulseVcoEnvironment* environment = (PulseVcoEnvironment*)malloc(sizeof(PulseVcoEnvironment));

    if(!environment)
        return (SignalSourceMono_f*)0;

    environment->duty_signal = duty_signal;

    return new_vco_wave(control_voltage, pulse_wave_function, environment);
}

