#include "../channel/channel.h"
#include "../config.h"
#include <math.h>

#define M_PI 3.14159265358979323846264338327
#define DEG_TO_RAD(x) (((x)*M_PI)/180.0)

typedef struct SineGeneratorEnv {
    float current_phase, //Position along the t axis in degrees
    float freq,          //Frequency of the generator
    float duration,      //How long to generate the signal in ms
    float time           //How long the signal has been running in ms
}

int sine_generator(float* sample, unsigned param_count, void* environment) {

    SineGeneratorEnvironment* vars = (SineGeneratorEnvironment*)environment;

    float deg_per_sample = (360.0 * vars->freq)/SAMPLE_RATE;

    vars->time += 1000.0/SAMPLE_RATE;

    if(vars->time >= duration) 
        return 0;

    vars->current_phase += deg_per_sample;

    if(vars->current_phase >= 360.0)
        vars->current_phase -= 360.0;

    sample[0] = sinf(DEG_TO_RAD(vars->current_phase));

    return 1;
}

SignalSourceMono_f* new_sine_test(float freq, float duration) {

    SignalSourceMono_f* signal = new_ssmf(sine_generator);
    
    if(!signal)
        return signal;

    SineGeneratorEnv* environment = (SineGeneratorEnv*)malloc(sizeof(SineGeneratorEnv));

    if(!environment) {
    
        //delete_ssmf(signal);
        return (SignalSourceMono_f*)0;
    }

    environment->current_phase = 0;
    environment->freq = freq;
    environment->duration = duration;
    environment->time = 0;

    signal->environment = environment;

    return signal;
}
