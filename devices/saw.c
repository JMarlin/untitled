#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "saw.h"
#include "wavegenerator.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#define DEG_TO_RAD(x) (((x)*M_PI)/180.0)
#define WAVE_SCALE 0.95 //At what % of the cycle the signal will start to drop back to 0

float saw_wave_function(float phase, void* environment) {

    return phase <= (360 * WAVE_SCALE) ?
           ((phase / 180.0) / WAVE_SCALE) - 1 :   //Upswing
           (-2 * (phase - (360 * WAVE_SCALE)) / (360 * (1 - WAVE_SCALE))) + 1; //Downswing
}

SignalSourceMono_f* new_fixed_saw(float freq) {

    return new_fixed_wave(freq, saw_wave_function, (void*)0);
}

SignalSourceMono_f* new_saw_vco(ControlVoltage* control_voltage) {

    return new_vco_wave(control_voltage, saw_wave_function, (void*)0);
}

