#ifndef SPLIT_H
#define SPLIT_H

#include "../channel/channel.h"
#include "controlvoltage.h"

void new_cv_split(ControlVoltage* input_cv, ControlVoltage** out_cv_a, ControlVoltage** out_cv_b);
void new_mf_split(SignalSourceMono_f* input_signal, SignalSourceMono_f** out_signal_1, SignalSourceMono_f** out_signal_2);

#endif //SPLIT_H
