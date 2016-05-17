#include "controlvoltage.h"
#include "../channel/channel.h"
#include "sequencer.h"
#include <inttypes.h>
#include <stdlib.h>

typedef struct CvFromSequencerGeneratorEnv {
    Sequencer* sequencer;
    unsigned note_on;
    uint8_t current_note;
} CvFromSequencerGeneratorEnv;

int cv_from_sequencer_generator(float* pitch_sample, float* gate_sample, void* environment) {

    CvFromSequencerGeneratorEnv* vars = (CvFromSequencerGeneratorEnv*)environment;
    SequenceMessageCollection messages;
    int i;
    
    if(!sequencer_pull_next_sample(vars->sequencer, &messages))
        return 0;
    
    //Since a CV can only represent a single note at a time, we
    //take the first event in the list and reject the rest
    for(i = 0; i < messages.message_count; i++) {

        switch(messages.item[i]->action) {
        
            case SEQ_ACTON:
                vars->current_note = messages.item[i]->note;
                vars->note_on = 1;
                break;

            case SEQ_ACTOFF:
                if(vars->current_note == messages.item[i]->note)
                    vars->note_on = 0;
                break;

            default:
                break;
        }

        //Make sure we free the memory of the consumed
        //messages when we're finished with them
        free(messages.item[i]);
    }

    //Free the consumed array of message pointers
    free(messages.item);

    if(vars->note_on) {

        //Convert the note into a pitch signal
        //Note: right now, we only support notes 0-96
        //anything else will cause a pitch signal of over 1.0
        pitch_sample[0] = (float)vars->current_note;
        pitch_sample[0] *= 0.02083333333;
        pitch_sample[0] -= 1;
    }

    gate_sample[0] = vars->note_on ? 1.0 : -1.0;

    return 1;
}

ControlVoltage* new_cv_from_sequencer(Sequencer* sequencer) {

    ControlVoltage* control_voltage = (ControlVoltage*)malloc(sizeof(ControlVoltage));

    if(!control_voltage)
        return control_voltage;

    control_voltage->generator = cv_from_sequencer_generator;
    CvFromSequencerGeneratorEnv* environment = (CvFromSequencerGeneratorEnv*)malloc(sizeof(CvFromSequencerGeneratorEnv));

    if(!environment) {
    
        free(control_voltage);
        return (ControlVoltage*)0;
    }

    environment->sequencer = sequencer;
    environment->note_on = 0;
    environment->current_note = 0;
    control_voltage->environment = environment;

    return control_voltage;
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

