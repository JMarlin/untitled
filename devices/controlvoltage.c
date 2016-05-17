#include "controlvoltage.h"
#include "../channel/channel.h"
#include "sequencer.h"
#include <inttypes.h>
#include <stdlib.h>

ControlVoltage* new_cv_from_sequencer(Sequencer* sequencer) {

    //TODO IMPLEMENT ME

    return (ControlVoltage*)0;
}

typedef struct CvFromSsmfGeneratorEnv {
    SignalSourceMono_f* pitch_signal;
    SignalSourceMono_f* gate_signal;
} CvFromSsmfGeneratorEnv;

int cv_from_ssmf_generator(float* pitch_sample, float* gate_sample, void* environment) {

    CvFromSsmfGeneratorEnv* vars = (CvFromSsmfGeneratorEnv*)environment;

    int pitch_running = ssmf_pull_next_sample(vars->pitch_signal, pitch_sample);
    int gate_running = ssmf_pull_next_sample(vars->gate_signal, gate_sample);

    return pitch_running && gate_running;
}

ControlVoltage* new_cv_from_ssmf(SignalSourceMono_f* pitch_signal, SignalSourceMono_f* gate_signal) {

    ControlVoltage* control_voltage = (ControlVoltage*)malloc(sizeof(ControlVoltage));

    if(!control_voltage)
        return control_voltage;

    control_voltage->generator = cv_from_ssmf_generator;
    CvFromSsmfGeneratorEnv* environment = (CvFromSsmfGeneratorEnv*)malloc(sizeof(CvFromSsmfGeneratorEnv));

    if(!environment) {

        free(control_voltage);
        return (ControlVoltage*)0;
    }

    environment->pitch_signal = pitch_signal;
    environment->gate_signal = gate_signal;
    control_voltage->environment = environment;

    return control_voltage;
}

int cv_pull_next_sample(ControlVoltage* control_voltage, float* pitch_sample, float* gate_sample) {

    return control_voltage->generator(
               pitch_sample,
               gate_sample,
               control_voltage->environment
           );
}

