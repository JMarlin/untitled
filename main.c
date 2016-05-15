#include "channel/channel.h"
#include "devices/sine.h"
#include "wavlib/wavlib.h"
#include "config.h"

int main(int argc, char* argv[]) {
    
    //create a 440hz sine signal lasting 2s
    if(!(SignalSourceMono_f* sine_signal = new_sine_test(440.0, 2000.0)))
        return 0;
    
    //convert the mono signal into a stereo signal 
    if(!(SignalSourceStereo_f* sine_stereo_signal = new_sssf_from_ssmf(sine_signal))) {
        
        //delete_sine_test(sine_signal);
        return 0;
    }
    
    //Create the control signals for a stereo channel 
    if(!(SignalSourceMono_f* channel_pan = new_const_signal_mf(0.0))) {
        
        //delete_sssf_from_ssmf(sine_stereo_signal);
        //delete_sine_test(sine_signal);
        return 0;
    }
    
    if(!(SignalSourceMono_f* channel_gain = new_const_signal_mf(0.0))) {
        
        //delete_sssf_from_ssmf(sine_stereo_signal);
        //delete_sine_test(sine_signal);
        //delete_const_signal_mf(channel_pan);
        return 0;
    }
    
    //Create a stereo float output channel for the stereo signal 
    if(!(StereoChannel_f* stereo_channel = new_scf(sine_stereo_signal, channel_pan, channel_gain))) {
        
        //delete_sssf_from_ssmf(sine_stereo_signal);
        //delete_sine_test(sine_signal);
        //delete_const_signal_mf(channel_pan);
        //delete_const_signal_mf(channel_gain);
        return 0;
    }
    
    //Convert the float channel into an i16 channel 
    if(!(StereoChannel_i16* i16_channel = new_sssi16_from_scf(stereo_channel))) {
        
        //delete_sssf_from_ssmf(sine_stereo_signal);
        //delete_sine_test(sine_signal);
        //delete_const_signal_mf(channel_pan);
        //delete_const_signal_mf(channel_gain);
        //delete_scf(stereo_channel);
        return 0;
    }
    
    //Finally, stream the i16 channel out to WAV
    write_wav_pcm16_stereo("sine_test.wav", i16_channel);
    
    //Clean up and return 
    //delete_sssf_from_ssmf(sine_stereo_signal);
    //delete_sine_test(sine_signal);
    //delete_const_signal_mf(channel_pan);
    //delete_const_signal_mf(channel_gain);
    //delete_scf(stereo_channel);
    return 0;
}
