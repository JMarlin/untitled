#ifndef WAVEGENERATOR_H
#define WAVEGENERATOR_H

#include "../config.h"
#include "controlvoltage.h"

typedef float (*WaveFunction)(float, void*);

SignalSourceMono_f* new_fixed_wave(float freq, WaveFunction wave_function, void* wave_environment);
SignalSourceMono_f* new_vco_wave(ControlVoltage* control_voltage, WaveFunction wave_function, void* wave_environment);

#endif //WAVEGENERATOR_H

