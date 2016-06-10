#ifndef SNARE_H
#define SNARE_H

#include "../channel/channel.h"
#include "../devices/controlvoltage.h"

SignalSourceMono_f* new_snare(ControlVoltage* control_voltage);

#endif //SNARE_H
