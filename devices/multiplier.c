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

typedef struct MultiplierGeneratorEnv_s {
    SignalSourceMono_f* signal_a;
    SignalSourceMono_f* signal_b;
} MultiplierGeneratorEnv;

int multiplier_generator(float* sample, void* environment) {

    MultiplierGeneratorEnv* vars = (MultiplierGeneratorEnv*)environment;
    float sample_a, sample_b;
    int a_running = ssmf_pull_next_sample(vars->signal_a, &sample_a);
    int b_running = ssmf_pull_next_sample(vars->signal_b, &sample_b);

    sample_a = a_running ? sample_a : 1;
    sample_b = b_running ? sample_b : 1;

    sample[0] = sample_a * sample_b;

    return a_running || b_running;
}

//For later, frees everything in the supplied multiplier's environment
void multiplier_deleter(void* environment) {

    MultiplierGeneratorEnv* vars = (MultiplierGeneratorEnv*)environment;
    //delete_ssmf(vars->signal_a);
    //delete_ssmf(vars->signal_b);
}

SignalSourceMono_f* new_multiplier(SignalSourceMono_f* signal_a, SignalSourceMono_f* signal_b) {

    MultiplierGeneratorEnv* environment = (MultiplierGeneratorEnv*)malloc(sizeof(MultiplierGeneratorEnv));

    if(!environment) {
        
        //delete_ssmf(signal_a);
        //delete_ssmf(signal_b);
        return (SignalSourceMono_f*)0;
    }

    environment->signal_a = signal_a;
    environment->signal_b = signal_b;

    SignalSourceMono_f* signal_out = new_ssmf(multiplier_generator); //, multiplier_deleter, environment);
    
    //Need to remove when ssmf creation function is updated
    signal_out->environment = environment;

    //If new_ssmf fails, it will automatically call the deleter callback to clean up the environment object
    return signal_out;
}

