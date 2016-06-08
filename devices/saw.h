#ifndef SAW_H
#define SAW_H

#include "../channel/channel.h"
#include "controlvoltage.h"

SignalSourceMono_f* new_fixed_saw(float freq);
SignalSourceMono_f* new_saw_vco(ControlVoltage* control_voltage);

#endif //SINE_H
