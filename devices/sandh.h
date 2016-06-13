#ifndef SANDH_H
#define SANDH_H

#include "../config.h"
#include "channel/channel.h"

SignalSourceMono_f* new_sandh(SignalSourceMono_f* source_signal, SignalSourceMono_f* trigger_signal);

#endif //SANDH_H

