#include "../channel/channel.h"
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

typedef struct FreqToCVGeneratorEnv_s {
    SignalSourceMono_f* signal;
} FreqToCVGeneratorEnv;

int freqtocv_generator(float* sample, void* environment) {

    FreqToCVGeneratorEnv* vars = (FreqToCVGeneratorEnv*)environment;
    float in_sample;
    int running = ssmf_pull_next_sample(vars->signal, &in_sample);

    sample[0] = (log2f(in_sample / 55) / 4) - 1;

    return running;
}

//For later, frees everything in the supplied converter's environment
void freqtocv_deleter(void* environment) {

    FreqToCVGeneratorEnv* vars = (FreqToCVGeneratorEnv*)environment;
    //delete_ssmf(vars->signal);
}

SignalSourceMono_f* new_frequency_to_cv(SignalSourceMono_f* signal) {

    FreqToCVGeneratorEnv* environment = (FreqToCVGeneratorEnv*)malloc(sizeof(FreqToCVGeneratorEnv));

    if(!environment) {
        
        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->signal = signal;

    SignalSourceMono_f* signal_out = new_ssmf(freqtocv_generator); //, multiplier_deleter, environment);
    
    //Needs to be removed once we've updated the ssmf creation function
    signal_out->environment = environment;

    //If new_ssmf fails, it will automatically call the deleter callback to clean up the environment object
    return signal_out;
}

