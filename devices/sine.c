#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "sine.h"
#include "wavegenerator.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#define DEG_TO_RAD(x) (((x)*M_PI)/180.0)

float sine_wave_function(float phase, void* environment) {

    return sinf(DEG_TO_RAD(phase));
}

SignalSourceMono_f* new_fixed_sine(float freq, float duration) {

    return new_fixed_wave(freq, duration, sine_wave_function, (void*)0);
}

SignalSourceMono_f* new_sine_vco(ControlVoltage* control_voltage) {

    return new_vco_wave(control_voltage, sine_wave_function, (void*)0);
}

