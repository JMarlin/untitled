#include "channel/channel.h"
#include "devices/sine.h"
#include "wavlib/wavlib.h"
#include "devices/sum.h"
#include "config.h"

#define ATTEMPT(x) if(!(x)) { cleanup(); return 0; }

//TODO update and implement cleanup code (might even just put everything in global scope and write one cleanup function)
//TODO right now there is no reason you couldn't feed a signal to multiple inputs, however with the way things are designed 
//     this would cause, if the signal is connected to two inputs for example, for the source to be effectively sampled 
//     at 2x the rate of the system. This must be avoided.

SignalSourceMono_f *sine_signal_l, *sine_signal_r, *channel_l_gain, *channel_l_pan, *channel_r_gain, *channel_r_pan, *master_gain, *master_pan;
SignalSourceStereo_f *sine_l_stereo_signal, *sine_r_stereo_signal, *mixer_signal;
StereoChannel_f *l_channel, *r_channel, *master_channel;
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
    
    
    
    //create a 440hz sine signal lasting 2s
    ATTEMPT(sine_signal_l = new_sine_test(440.0, 10000.0));
    
    //create a 554.37hz sine signal lasting 2s
    ATTEMPT(sine_signal_r = new_sine_test(554.37, 12000.0));
    
    //convert the mono signals into stereo signals 
    ATTEMPT(sine_l_stereo_signal = new_sssf_from_ssmf(sine_signal_l)); 
    ATTEMPT(sine_r_stereo_signal = new_sssf_from_ssmf(sine_signal_r));
    
    //Create the control signals for stereo channels 
    ATTEMPT(channel_l_pan = new_sine_test(5.0, 10000.0));
    ATTEMPT(channel_l_gain = new_const_signal_mf(0.0));
    ATTEMPT(channel_r_pan = new_const_signal_mf(1.0));
    ATTEMPT(channel_r_gain = new_const_signal_mf(0.0));
    
    //Create a channel for each signal 
    ATTEMPT(l_channel = new_scf(sine_l_stereo_signal, channel_l_pan, channel_l_gain));
    ATTEMPT(r_channel = new_scf(sine_r_stereo_signal, channel_r_pan, channel_r_gain));
    
    //Create a mixer to sum the two channels 
    ATTEMPT(mixer_signal = new_summer_sf(2));
    summer_sf_assign_input(mixer_signal, 1, l_channel);
    summer_sf_assign_input(mixer_signal, 2, r_channel);
    
    //Create a final master stereo channel from the mixer output
    ATTEMPT(master_pan = new_const_signal_mf(0.0));
    ATTEMPT(master_gain = new_const_signal_mf(0.0)); 
    ATTEMPT(master_channel = new_scf(mixer_signal, master_pan, master_gain));
    
    //Convert the float channel into an i16 signal 
    ATTEMPT(i16_signal = new_sssi16_from_scf(master_channel));
    
    //Wrap the i16 signal in an i16 channel
    ATTEMPT(i16_channel = new_sci16(i16_signal));
    
    //Finally, stream the i16 channel out to WAV
    write_wav_pcm16_stereo("sine_test.wav", i16_channel);
    
    return 0;
}
