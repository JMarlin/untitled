#ifndef ENVELOPE_SINE_H
#define ENVELOPE_SINE_H

#include "../channel/channel.h"
#include "../devices/controlvoltage.h"

SignalSourceMono_f* new_envelope_sine(ControlVoltage* control_voltage);

#endif //ENVELOPE_SINE_H
