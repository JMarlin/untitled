#include "channel/channel.h"
#include "devices/sine.h"
#include "devices/controlvoltage.h"
#include "devices/sequencer.h"
#include "wavlib/wavlib.h"
#include "devices/sum.h"
#include "config.h"

#define ATTEMPT(x) if(!(x)) { cleanup(); return 0; }

//TODO update and implement cleanup code (might even just put everything in global scope and write one cleanup function)
//TODO right now there is no reason you couldn't feed a signal to multiple inputs, however with the way things are designed 
//     this would cause, if the signal is connected to two inputs for example, for the source to be effectively sampled 
//     at 2x the rate of the system. This must be avoided.

ControlVoltage* control;
SignalSourceMono_f *sine_signal, *pitch_sweep, *gate_const, *master_gain, *master_pan;
SignalSourceStereo_f *sine_stereo_signal;
StereoChannel_f *master_channel;
SignalSourceStereo_i16 *i16_signal;
StereoChannel_i16 *i16_channel;

void cleanup() {
    
    //Clean up and return 
    //delete_sssf_from_ssmf(sine_stereo_signal);
    //delete_sine_test(sine_signal);
    //delete_const_signal_mf(channel_pan);
    //delete_const_signal_mf(channel_gain);
    //delete_scf(stereo_channel);
    //delete_sssi16_from_scf(i16_signal);
    //delete_sci16(i16_channel);
}

int main(int argc, char* argv[]) {
    
    
    
    //create a control voltage with a slow pitch sweep and set to constantly on
    ATTEMPT(pitch_sweep = new_fixed_sine(0.25, 9000.0));
    ATTEMPT(gate_const = new_const_signal_mf(1.0));
    ATTEMPT(control = new_cv_from_ssmf(pitch_sweep, gate_const));
    
    //create a sine vco controlled by the cv
    ATTEMPT(sine_signal = new_sine_vco(control));
    
    //convert the mono signal into a stereo signal 
    ATTEMPT(sine_stereo_signal = new_sssf_from_ssmf(sine_signal)); 
    
    //Create a final master stereo channel from the mixer output
    ATTEMPT(master_pan = new_fixed_sine(1.0, 9000.0));
    ATTEMPT(master_gain = new_const_signal_mf(1.0)); 
    ATTEMPT(master_channel = new_scf(sine_stereo_signal, master_pan, master_gain));
    
    //Convert the float channel into an i16 signal 
    ATTEMPT(i16_signal = new_sssi16_from_scf(master_channel));
    
    //Wrap the i16 signal in an i16 channel
    ATTEMPT(i16_channel = new_sci16(i16_signal));
    
    //Finally, stream the i16 channel out to WAV
    write_wav_pcm16_stereo("untitled.wav", i16_channel);
    
    return 0;
}
