#ifndef CONTROLVOLTAGE_H
#define CONTROLVOLTAGE_H

#include "../channel/channel.h"
#include "sequencer.h"
#include <inttypes.h>

typedef int(*ControlVoltageGeneratorFunction)(float*, float*, void*);

typedef struct ControlVoltage {
    ControlVoltageGeneratorFunction generator;
    void* environment;
} ControlVoltage;

ControlVoltage* new_cv_from_sequencer(Sequencer* sequencer);
ControlVoltage* new_cv_from_ssmf(SignalSourceMono_f* pitch_signal, SignalSourceMono_f* gate_signal);
int cv_pull_next_sample(ControlVoltage* control_voltage, float* pitch_sample, float* gate_sample);

#endif //CONTROLVOLTAGE_H
