#ifndef FORMANT_H
#define FORMANT_H

#include "../channel/channel.h"

SignalSourceMono_f* new_formant_filter(SignalSourceMono_f* input_signal, int vowelnum);

#endif //FORMANT_H
