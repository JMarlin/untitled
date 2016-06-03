#ifndef KICK_H
#define KICK_H

#include "../channel/channel.h"
#include "../devices/controlvoltage.h"

SignalSourceMono_f* new_kick(ControlVoltage* control_voltage);

#endif //KICK_H
