#ifndef ADSR_H
#define ADSR_H

#include "../channel/channel.h"

SignalSourceMono_f* new_adsr(SignalSourceMono_f* input_signal, SignalSourceMono_f* a_signal, SignalSourceMono_f* d_signal, SignalSourceMono_f* s_signal, SignalSourceMono_f* r_signal);

#endif //ADSR_H
