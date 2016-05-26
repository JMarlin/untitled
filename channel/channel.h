#ifndef CHANNEL_H
#define CHANNEL_H

#include <inttypes.h>

//floating-point channels
typedef struct SignalSourceMono_f SignalSourceMono_f;
typedef struct SignalSourceStereo_f SignalSourceStereo_f;

typedef int(*GeneratorFunctionMono_f)(float*, void*);
typedef int(*GeneratorFunctionStereo_f)(float*, float*, void*);

typedef struct SignalSourceMono_f {
    GeneratorFunctionMono_f generator;
    void* environment;
} SignalSourceMono_f;

typedef struct SignalSourceStereo_f {
    GeneratorFunctionStereo_f generator;
    void* environment;
} SignalSourceStereo_f;

typedef struct MonoChannel_f {
    SignalSourceMono_f* signal;
    SignalSourceMono_f* gain_signal;
    float last_gain;
} MonoChannel_f;

typedef struct StereoChannel_f {
    SignalSourceStereo_f* signal;
    SignalSourceMono_f* pan_signal;
    SignalSourceMono_f* gain_signal;
    float last_pan;
    float last_gain;
} StereoChannel_f;

int scf_pull_next_sample(StereoChannel_f* stereo_channel, float* l_sample, float* r_sample);
int mcf_pull_next_sample(MonoChannel_f* mono_channel, float* sample);
int sssf_pull_next_sample(SignalSourceStereo_f* signal, float* l_sample, float* r_sample);
int ssmf_pull_next_sample(SignalSourceMono_f* signal, float* sample);

//16-bit signed int channels
typedef struct SignalSourceMono_i16 SignalSourceMono_i16;
typedef struct SignalSourceStereo_i16 SignalSourceStereo_i16;

typedef int(*GeneratorFunctionMono_i16)(int16_t*, void*);
typedef int(*GeneratorFunctionStereo_i16)(int16_t*, int16_t*, void*);

typedef struct SignalSourceMono_i16 {
    GeneratorFunctionMono_i16 generator;
    void* environment;
} SignalSourceMono_i16;

typedef struct SignalSourceStereo_i16 {
    GeneratorFunctionStereo_i16 generator;
    void* environment;
} SignalSourceStereo_i16;

typedef struct MonoChannel_i16 {
    SignalSourceMono_i16* signal;
} MonoChannel_i16;

typedef struct StereoChannel_i16 {
    SignalSourceStereo_i16* signal;
} StereoChannel_i16;

int sci16_pull_next_sample(StereoChannel_i16* stereo_channel, int16_t* l_sample, int16_t* r_sample);
int mci16_pull_next_sample(MonoChannel_i16* mono_channel, int16_t* sample);
int sssi16_pull_next_sample(SignalSourceStereo_i16* signal, int16_t* l_sample, int16_t* r_sample);
int ssmi16_pull_next_sample(SignalSourceMono_i16* signal, int16_t* sample);

//Signal and channel creation methods
//StereoChannel_f* new_scf_from_sssf(SignalSourceStereo_f* signal, SignalSourceMono_f* pan_signal, SignalSourceMono_f* gain_signal);
//^ Need the above for stereo and mono float and i16 channels
//Also need, at the very least, a function to convert (downsample) a stereo float channel into a stereo i16 channel
SignalSourceMono_f* new_ssmf(GeneratorFunctionMono_f generator);
SignalSourceStereo_f* new_sssf_from_ssmf(SignalSourceMono_f* mono_signal);
SignalSourceStereo_i16* new_sssi16_from_scf(StereoChannel_f* stereo_channel_f);
SignalSourceStereo_f* new_sssf(GeneratorFunctionStereo_f generator);
SignalSourceStereo_i16* new_sssi16(GeneratorFunctionStereo_i16 generator);
StereoChannel_f* new_scf(SignalSourceStereo_f* audio_signal, SignalSourceMono_f* pan_signal, SignalSourceMono_f* gain_signal);
SignalSourceMono_f* new_const_signal_mf(float value, float duration);
StereoChannel_i16* new_sci16(SignalSourceStereo_i16* signal);

#endif //CHANNEL_H
