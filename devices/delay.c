#include "../channel/channel.h"
#include "../util.h"
#include "../config.h"
#include <inttypes.h>
#include <math.h>

typedef struct DelayGeneratorEnv {
    SignalSourceMono_f* input_signal;
    SignalSourceMono_f* delay_signal;
    SignalSourceMono_f* feedback_signal;
    SignalSourceMono_f* blend_signal;
    uint32_t buffer_size;
    float* delay_buffer;
    float max_time;
    uint32_t play_position;
} DelayGeneratorEnv;

int delay_deleter(void* environment) {

   //vars
   //if(vars->input_signal) ssmf_delete(vars->input_signal);
   //if(vars->delay_signal) ssmf_delete(vars->delay_signal);
   //if(vars->feedback_signal) ssmf_delete(vars->feedback_signal);
   //if(vars->blend_signal) ssmf_delete(vars->blend_signal);
   //if(vars->delay_buffer) free(vars->delay_buffer);

   return 1; 
}

int delay_generator(float* sample, void* environment) {

    DelayGeneratorEnv* vars = (DelayGeneratorEnv*)environment;

    float input_sample, delay_sample, feedback_sample, blend_sample;
    int input_running = ssmf_pull_next_sample(vars->input_signal, &input_sample);
    int feedback_running = ssmf_pull_next_sample(vars->feedback_signal, &feedback_sample);
    int delay_running = ssmf_pull_next_sample(vars->delay_signal, &delay_sample);
    int blend_running = ssmf_pull_next_sample(vars->blend_signal, &blend_sample);

    if(!(input_running && feedback_running && delay_running && blend_running)) {
         
        sample[0] = 0;
        return 0;
    }

    uint32_t delay_samples = (uint32_t)floorf((((delay_sample + 1) / 2) * vars->max_time) / MS_PER_SAMPLE);
    uint32_t record_position = (vars->play_position + delay_samples) % vars->buffer_size;
    
    sample[0] = vars->delay_buffer[vars->play_position] + input_sample;
    vars->delay_buffer[record_position] = (((feedback_sample + 1) / 2) * sample[0]);
    sample[0] = (vars->delay_buffer[vars->play_position] * ((blend_sample + 1) / 2)) + (input_sample * ((1 - blend_sample) / 2));

    vars->play_position = (vars->play_position + 1) % vars->buffer_size;

    return 1;
}

SignalSourceMono_f* new_delay(SignalSourceMono_f* input_signal, SignalSourceMono_f* delay_signal, SignalSourceMono_f* feedback_signal, SignalSourceMono_f* blend_signal, float max_time) {

    DelayGeneratorEnv* environment = new(DelayGeneratorEnv);

    if(!environment) 
        return (SignalSourceMono_f*)0;

    environment->input_signal = input_signal;
    environment->feedback_signal = feedback_signal;
    environment->delay_signal = delay_signal;
    environment->blend_signal = blend_signal;
    environment->play_position = 0;
    environment->max_time = max_time;
    environment->buffer_size = (uint32_t)ceilf((max_time * 2) / MS_PER_SAMPLE);

    environment->delay_buffer = (float*)malloc(environment->buffer_size * sizeof(float));

    if(!environment->delay_buffer) {
    
        //delay_deleter(environment);
        //free(environment);
        return (SignalSourceMono_f*)0;
    }

    int i;
    for(i = 0; i < environment->buffer_size; i++)
        environment->delay_buffer[i] = 0;

    SignalSourceMono_f* out_signal = new_ssmf(delay_generator); //delay_deleter, environment);

    //Delete
    if(out_signal)
        out_signal->environment = environment;
    
    return out_signal;
}
