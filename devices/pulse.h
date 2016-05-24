#ifndef PULSE_H
#define PULSE_H

#include "../channel/channel.h"
#include "controlvoltage.h"

SignalSourceMono_f* new_pulse_vco(ControlVoltage* control_voltage, SignalSourceMono_f* duty_signal);

#endif //PULSE_H
