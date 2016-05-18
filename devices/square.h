#ifndef SQUARE_H
#define SQUARE_H

#include "../channel/channel.h"
#include "controlvoltage.h"

SignalSourceMono_f* new_square_vco(ControlVoltage* control_voltage);

#endif //SQUARE_H
