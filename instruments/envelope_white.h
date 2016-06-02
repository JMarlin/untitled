#ifndef ENVELOPE_WHITE_H
#define ENVELOPE_WHITE_H

#include "../channel/channel.h"
#include "../devices/controlvoltage.h"

SignalSourceMono_f* new_envelope_white(ControlVoltage* control_voltage);

#endif //ENVELOPE_WHITE_H
