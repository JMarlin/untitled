#ifndef CVMULTIPLIER_H
#define CVMULTIPLIER_H

#include "../channel/channel.h"
#include "controlvoltage.h"

ControlVoltage* new_cv_pitch_multiplier(ControlVoltage* control_voltage, SignalSourceMono_f* multiplier_signal);

#endif //CVMULTIPLIER_H

