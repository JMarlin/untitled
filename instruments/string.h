#ifndef STRING_H
#define STRING_H

#include "../channel/channel.h"
#include "../devices/controlvoltage.h"

SignalSourceMono_f* new_string(ControlVoltage* control_voltage);

#endif //STRING_H
