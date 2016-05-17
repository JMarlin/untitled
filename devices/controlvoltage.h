#ifndef CONTROLVOLTAGE_H
#define CONTROLVOLTAGE_H

#include "../channel.h"
#include "sequencer.h"
#include <inttypes.h>

typedef int(ControlVoltageGeneratorFunction*)(float*, float*, void*);

typedef struct ControlVoltage {
    ControlVoltageGeneratorFunction generator;
    void* environment;
}

ControlVoltage* cv_from_sequencer(Sequencer* sequencer);
ControlVoltage* cv_from_ssmf(SignalSourceMono_f* pitch_signal, SignalSourceMono_f* gate_signal);
int cv_pull_next_sample(float* pitch_sample, float* gate_sample, ControlVoltage* control_voltage);

#endif //CONTROLVOLTAGE_H
