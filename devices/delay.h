#ifndef DELAY_H
#define DELAY_H

#include "../channel/channel.h"

SignalSourceMono_f* new_delay(SignalSourceMono_f* input_signal, SignalSourceMono_f* delay_signal, SignalSourceMono_f* feedback_signal, SignalSourceMono_f* blend_signal, float max_time);

#endif //DELAY_H
