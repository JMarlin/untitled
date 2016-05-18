#include "../channel/channel.h"
#include "../config.h"
#include "controlvoltage.h"
#include "square.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#define DEG_TO_RAD(x) (((x)*M_PI)/180.0)

typedef struct SquareVcoGeneratorEnv {
    float current_phase;
    unsigned last_state;
    ControlVoltage* control_voltage;
} SquareVcoGeneratorEnv;

//This is almost identical to the sine vco save for the expression beginning sample[0],
//and we will probably use this pattern again, so it would probably behoove us to abstract
//this into a more generalized module that lets us pass a wave-calculator function that
//spits out an amplitude given the current phase
int square_vco_generator(float* sample, void* environment) {
    
    SquareVcoGeneratorEnv* vars = (SquareVcoGeneratorEnv*)environment;
    float cv_pitch_sample, cv_gate_sample;

    //If the control signal drops, we drop
    if(!cv_pull_next_sample(vars->control_voltage, &cv_pitch_sample, &cv_gate_sample)) {
        
        sample[0] = 0;
        return 0;
    }
    
    //Control voltages assume a volt-to-hertz scheme covering eight
    //octaves from A0 to A8  mapped over -1.0 to 1.0
    //(eg: -1.0 = 55hz, 0.0 = 880hz, 1.0 = 14080hz)
    if(cv_gate_sample > -1.0) { 

        if(!vars->last_state) {

            vars->current_phase = 90.0;
            vars->last_state = 1;
        }

        float freq = 55.0 * pow(2, 8.0*(1.0 + cv_pitch_sample)/2.0);
        float deg_per_sample = (360.0 * freq)/SAMPLE_RATE;

        sample[0] = vars->current_phase >= 180.0 ? 1.0 : 0.0; 

        vars->current_phase += deg_per_sample;

        if(vars->current_phase >= 360.0)
            vars->current_phase -= 360.0;
    } else {
   
        sample[0] = 0;
        vars->last_state = 0;
    }

    return 1;
}

SignalSourceMono_f* new_square_vco(ControlVoltage* control_voltage) {

    SignalSourceMono_f* signal = new_ssmf(square_vco_generator);

    if(!signal)
        return signal;

    SquareVcoGeneratorEnv* environment = (SquareVcoGeneratorEnv*)malloc(sizeof(SquareVcoGeneratorEnv));

    if(!environment) {

        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->current_phase = 90.0;
    environment->last_state = 0;
    environment->control_voltage = control_voltage;
    
    signal->environment = environment;

    return signal;
}

