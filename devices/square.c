#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "square.h"
#include <math.h>
#include <stdlib.h>

float square_wave_function(float phase, void* environment) {
    
    return phase >= 180.0 ? 1.0 : -1.0; 
}

SignalSourceMono_f* new_square_vco(ControlVoltage* control_voltage) {

    return new_vco_wave(control_voltage, square_wave_function, (void*)0);
}

