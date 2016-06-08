#include "../channel/channel.h"
#include "controlvoltage.h"
#include "../config.h"
#include <stdlib.h>
#include <math.h>

/*
This file is the posterchild for how we want to implement our heirarchical
resource management and deletion scheme. We will change our device creation
functions to attempt environment initialization first, with the protocol to 
delete any inputs before returning in the failure case. This should cause
the deletion of owned signals to ripple down the signal path. We will 
genericize the deletion of ssmf &c. objects by also supplying a deletion 
callback to be inserted into the signal object. When the generic signal 
deletion function is called upon a passed signal object, the generic 
deletion function will call that callback on the object's generator
environment which will cause another cascading ripple as deletion 
functions are called on any owned objects in the environment. Since we are
passing this deletion callback on generic signal init, by initializing the
environment first on device creation, we will have the signal creation 
function use the callback to delete the environment as a free bonus in the
case that signal creation fails.
*/

typedef struct CVMultiplierGeneratorEnv_s {
    ControlVoltage* control_voltage;
    SignalSourceMono_f* multiplier_signal;
} CVMultiplierGeneratorEnv;

int cv_multiplier_generator(float* pitch_sample, float* gate_sample, void* environment) {

    CVMultiplierGeneratorEnv* vars = (CVMultiplierGeneratorEnv*)environment;
    float multiplier_sample;
    int cv_running = cv_pull_next_sample(vars->control_voltage, pitch_sample, gate_sample);
    int multiplier_running = ssmf_pull_next_sample(vars->multiplier_signal, &multiplier_sample);

    if(!(cv_running && multiplier_running))
        return 0;

    pitch_sample[0] = pitch_sample[0] * multiplier_sample;

    return 1;
}

//For later, frees everything in the supplied multiplier's environment
void cv_multiplier_deleter(void* environment) {

    CVMultiplierGeneratorEnv* vars = (CVMultiplierGeneratorEnv*)environment;
    //delete_ssmf(vars->signal_a);
    //delete_ssmf(vars->signal_b);
}

ControlVoltage* new_cv_pitch_multiplier(ControlVoltage* control_voltage, SignalSourceMono_f* multiplier_signal) {

    CVMultiplierGeneratorEnv* environment = (CVMultiplierGeneratorEnv*)malloc(sizeof(CVMultiplierGeneratorEnv));

    if(!environment) {
        
        //delete_ssmf(signal_a);
        //delete_ssmf(signal_b);
        return (ControlVoltage*)0;
    }

    environment->control_voltage = control_voltage;
    environment->multiplier_signal = multiplier_signal;

    ControlVoltage* cv_out = (ControlVoltage*)malloc(sizeof(ControlVoltage));

    if(!cv_out) {

        free(environment);
        return cv_out;
    }

    cv_out->generator = cv_multiplier_generator;
    cv_out->environment = environment;
    
    //If new_ssmf fails, it will automatically call the deleter callback to clean up the environment object
    return cv_out;
}

