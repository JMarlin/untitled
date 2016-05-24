#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "white.h"
#include "wavegenerator.h"
#include <math.h>
#include <stdlib.h>

float white_wave_function(float phase, void* environment) {
    
    return -1.0 + 2.0*((float)rand())/RAND_MAX; 
}

SignalSourceMono_f* new_white_noise(float duration) {

    return new_fixed_wave(0, duration, white_wave_function, (void*)0);
}

