#ifndef SPLIT_H
#define SPLIT_H

#include "../channel/channel.h"

void new_split(SignalSourceMono_f* input_signal, SignalSourceMono_f** out_signal_a, SignalSourceMono_f** out_signal_b);

#endif SPLIT_H
