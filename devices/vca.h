#ifndef VCA_H
#define VCA_H

#include "../channel/channel.h"

SignalSourceMono_f* new_vca(SignalSourceMono_f* input_signal, SignalSourceMono_f* v_signal);

#endif //VCA_H
