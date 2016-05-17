#ifndef SINE_H
#define SINE_H

#include "../channel/channel.h"
#include "controlvoltage.h"

SignalSourceMono_f* new_fixed_sine(float freq, float duration);
SignalSourceMono_f* new_sine_vco(ControlVoltage* control_voltage);

#endif //SINE_H
