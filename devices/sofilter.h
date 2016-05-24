#ifndef SOFILTER_H
#define SOFILTER_H

#include "../channel/channel.h"

int so_filter_generator(float* sample, void* environment);
SignalSourceMono_f* new_so_filter(SignalSourceMono_f* input_signal, float b0, float b1, float b2, float a1, float a2);

#endif //SOFILTER_H
