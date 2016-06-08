#include "channel.h"
#include "../config.h"
#include <inttypes.h>
#include <stdlib.h>

int ssmi16_pull_next_sample(SignalSourceMono_i16* signal, int16_t* sample) {

    return signal->generator(
               sample,
               signal->environment
           );
}

int sssi16_pull_next_sample(SignalSourceStereo_i16* signal, int16_t* l_sample, int16_t* r_sample) {
    
    return signal->generator(
               l_sample,
               r_sample,
               signal->environment
           );
}

int ssmf_pull_next_sample(SignalSourceMono_f* signal, float* sample) {

    return signal->generator(
               sample,
               signal->environment
           ); 
}

int sssf_pull_next_sample(SignalSourceStereo_f* signal, float* l_sample, float* r_sample) {

    return signal->generator(
               l_sample,
               r_sample,
               signal->environment
           );
}

int sci16_pull_next_sample(StereoChannel_i16* stereo_channel, int16_t* l_sample, int16_t* r_sample) {

    int generator_running = sssi16_pull_next_sample(stereo_channel->signal, l_sample, r_sample);

    //Suppress any garbage samples from the generator
    if(!generator_running) {
        l_sample[0] = 0;
        r_sample[0] = 0;
    }

    return generator_running;
}

int mci16_pull_next_sample(MonoChannel_i16* mono_channel, int16_t* sample) {

    int generator_running = ssmi16_pull_next_sample(mono_channel->signal, sample);
    
    //Suppress any garbage samples from the generator
    if(!generator_running)
        sample[0] = 0;

    return generator_running;
}

int mcf_pull_next_sample(MonoChannel_f* mono_channel, float* sample) {
    
    float gain_sample;
    int generator_running = ssmf_pull_next_sample(mono_channel->signal, sample);
    int gain_running = ssmf_pull_next_sample(mono_channel->gain_signal, &gain_sample);

    //Freeze gain setting when gain signal drops
    if(gain_running)
        mono_channel->last_gain = gain_sample;
    else
        gain_sample = mono_channel->last_gain;

    //Apply gain to the output sample
    float gain = (1.0 + gain_sample)/2.0;
    sample[0] = sample[0] * gain;

    //Finally, apply clipping
    if(sample[0] > 1)
        sample[0] = 1;

    if(sample[0] < -1)
        sample[0] = -1;

    return generator_running;
}

int scf_pull_next_sample(StereoChannel_f* stereo_channel, float* l_sample, float* r_sample) {

    float pan_sample, gain_sample;
    int generator_running = sssf_pull_next_sample(stereo_channel->signal, l_sample, r_sample);
    int pan_running = ssmf_pull_next_sample(stereo_channel->pan_signal, &pan_sample);
    int gain_running = ssmf_pull_next_sample(stereo_channel->gain_signal, &gain_sample); 

    //Freeze pan setting when pan signal drops
    if(pan_running)
        stereo_channel->last_pan = pan_sample;
    else
        pan_sample = stereo_channel->last_pan;

    //Freeze gain setting when gain signal drops
    if(gain_running)
        stereo_channel->last_gain = gain_sample;
    else
        gain_sample = stereo_channel->last_gain;

    //Adjust output samples for the channel's pan and gain
    //Start by panning (gain per channel)
    float l_gain = (1.0 - pan_sample)/2.0;
    float r_gain = (1.0 + pan_sample)/2.0;
    l_sample[0] = l_sample[0] * l_gain;
    r_sample[0] = r_sample[0] * r_gain; 

    //FIXME the below gain calculations aren't accurate to 
    //how we want to actually model gain controls
    //Then apply gain to both samples 
    float gain = (1.0 + gain_sample)/2.0;
    l_sample[0] = l_sample[0] * gain;
    r_sample[0] = r_sample[0] * gain;

    //Finally, apply clipping
    if(l_sample[0] > 1)
        l_sample[0] = 1;
    
    if(l_sample[0] < -1)
        l_sample[0] = -1;

    if(r_sample[0] > 1)
        r_sample[0] = 1;
    
    if(r_sample[0] < -1)
        r_sample[0] = -1;

    //Stop only once both channels have stopped generating samples
    return generator_running;
}

typedef struct SsmfToSssfGeneratorEnv {
    SignalSourceMono_f* input_signal;
} SsmfToSssfGeneratorEnv;

int ssmf_to_sssf_generator(float* l_sample, float* r_sample, void* environment) {

    float sample;
    
    SsmfToSssfGeneratorEnv* vars = (SsmfToSssfGeneratorEnv*)environment;

    int generator_running = ssmf_pull_next_sample(vars->input_signal, &sample);

    l_sample[0] = sample;
    r_sample[0] = sample;
    
    return generator_running;
}

typedef struct ScfToSssi16GeneratorEnv {
    StereoChannel_f* input_channel;
} ScfToSssi16GeneratorEnv;

int scf_to_sssi16_generator(int16_t* l_sample, int16_t* r_sample, void* environment) {

    float l_sample_f, r_sample_f;
    
    ScfToSssi16GeneratorEnv* vars = (ScfToSssi16GeneratorEnv*)environment;

    int running = scf_pull_next_sample(vars->input_channel, &l_sample_f, &r_sample_f);
    
    l_sample[0] = (int16_t)(32767 * l_sample_f);
    r_sample[0] = (int16_t)(32767 * r_sample_f);
    
    return running; 
}

SignalSourceStereo_i16* new_sssi16(GeneratorFunctionStereo_i16 generator) {

    SignalSourceStereo_i16* signal = (SignalSourceStereo_i16*)malloc(sizeof(SignalSourceStereo_i16));

    if(!signal)
        return signal;

    signal->generator = generator;

    return signal;
}

SignalSourceStereo_f* new_sssf(GeneratorFunctionStereo_f generator) {

    SignalSourceStereo_f* signal = (SignalSourceStereo_f*)malloc(sizeof(SignalSourceStereo_f));

    if(!signal)
        return signal;

    signal->generator = generator;

    return signal;
}

SignalSourceMono_f* new_ssmf(GeneratorFunctionMono_f generator) {

    SignalSourceMono_f* signal = (SignalSourceMono_f*)malloc(sizeof(SignalSourceMono_f));

    if(!signal)
        return signal;

    signal->generator = generator;

    return signal;
}

SignalSourceStereo_f* new_sssf_from_ssmf(SignalSourceMono_f* mono_signal) {
    
    SignalSourceStereo_f* stereo_signal = new_sssf(ssmf_to_sssf_generator);

    if(!stereo_signal) 
        return stereo_signal;

    SsmfToSssfGeneratorEnv* environment = (SsmfToSssfGeneratorEnv*)malloc(sizeof(SsmfToSssfGeneratorEnv));

    if(!environment) {
        
        //delete_sssf(stereo_signal);
        return (SignalSourceStereo_f*)0;
    }

    environment->input_signal = mono_signal;
    stereo_signal->environment = environment;
    
    return stereo_signal;
}

SignalSourceStereo_i16* new_sssi16_from_scf(StereoChannel_f* stereo_channel_f) {

    SignalSourceStereo_i16* signal_i16 = new_sssi16(scf_to_sssi16_generator);

    if(!signal_i16)
        return signal_i16;

    ScfToSssi16GeneratorEnv* environment = (ScfToSssi16GeneratorEnv*)malloc(sizeof(ScfToSssi16GeneratorEnv));

    if(!environment) {

        //delete_sssi16(signal_i16);
        return (SignalSourceStereo_i16*)0;
    }

    environment->input_channel = stereo_channel_f;    
    signal_i16->environment = environment;

    return signal_i16;
}

StereoChannel_f*  new_scf(SignalSourceStereo_f* audio_signal, SignalSourceMono_f* pan_signal, SignalSourceMono_f* gain_signal) {
    
    StereoChannel_f* stereo_channel = (StereoChannel_f*)malloc(sizeof(StereoChannel_f));
    
    if(!stereo_channel)
        return stereo_channel;
        
    stereo_channel->signal = audio_signal;
    stereo_channel->pan_signal = pan_signal;
    stereo_channel->gain_signal = gain_signal;
    stereo_channel->last_pan = 0.0;
    stereo_channel->last_gain = 0.0;
    
    return stereo_channel;
}

typedef struct ConstSsmfGeneratorEnv {
    float value;
} ConstSsmfGeneratorEnv;

int const_ssmf_generator(float* sample, void* environment) {
    
    ConstSsmfGeneratorEnv* vars = (ConstSsmfGeneratorEnv*)environment;

    sample[0] = vars->value;
    
    return 1;
}

SignalSourceMono_f* new_const_signal_mf(float value) {
    
    SignalSourceMono_f* const_signal = new_ssmf(const_ssmf_generator);
    
    if(!const_signal)
        return const_signal;
    
    ConstSsmfGeneratorEnv* environment = (ConstSsmfGeneratorEnv*)malloc(sizeof(ConstSsmfGeneratorEnv));
    
    if(!environment) {
        
        //delete_ssmf(const_signal)
        return (SignalSourceMono_f*)0;
    }
    
    environment->value = value;
    const_signal->environment = environment;
    
    return const_signal;
}

StereoChannel_i16* new_sci16(SignalSourceStereo_i16* signal) {
    
    StereoChannel_i16* channel = (StereoChannel_i16*)malloc(sizeof(StereoChannel_i16));
    
    if(!channel)
        return channel;
        
    channel->signal = signal;
    
    return channel;
}
