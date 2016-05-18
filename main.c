#include "channel/channel.h"
#include "devices/sine.h"
#include "devices/square.h"
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

ControlVoltage *m_control, *b_control;
Sequencer *m_sequencer, *b_sequencer;
SignalSourceMono_f *m_sine_signal, *b_sine_signal,
                   *m_gain, *m_pan, *b_gain, *b_pan,
                   *master_gain, *master_pan;
SignalSourceStereo_f *mixer, *m_sine_stereo_signal, *b_sine_stereo_signal;
StereoChannel_f *m_channel, *b_channel, *master_channel;
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

//Would be nice if we could just plug a tree of functions into other 
//functions and then have the constructors fail gracefully if they
//recieve a null argument so that the failure ripples up the tree
//and ends up with one safe failed allocation check
//So this could be:
//
//master_channel = new_scf(
//                     new_sssf_from_ssmf(
//                         new_sine_vco(
//                             new_cv_from_sequencer(b_sequencer)
//                         )
//                     ),
//                     new_const_signal_mf(0.0),
//                     new_const_signal_mf(1.0)
//                 );
//
//Then, as an added bonus, we could add the capacity for destructors
//to ripple down (which makes sense since things only connect in
//one direction so objects always own their inputs) so that 
//calling delete_scf(master_channel); would also free everything
//attached to it

int main(int argc, char* argv[]) {
    
    //Create a seqencer, plug the seqencer into a CV converter
    //and finally plug the CV converter into a sine VCO
    ATTEMPT(m_sequencer = new_sequencer()); 
    ATTEMPT(m_control = new_cv_from_sequencer(m_sequencer));
    ATTEMPT(m_sine_signal = new_sine_vco(m_control));
    ATTEMPT(b_sequencer = new_sequencer());
    ATTEMPT(b_control = new_cv_from_sequencer(b_sequencer));
    ATTEMPT(b_sine_signal = new_square_vco(b_control));
    
    //convert the mono signal into a stereo signal 
    ATTEMPT(m_sine_stereo_signal = new_sssf_from_ssmf(m_sine_signal)); 
    ATTEMPT(b_sine_stereo_signal = new_sssf_from_ssmf(b_sine_signal));    

    //Wrap the signals into stereo channels for mixing
    ATTEMPT(m_pan = new_const_signal_mf(0.0));
    ATTEMPT(m_gain = new_const_signal_mf(1.0));
    ATTEMPT(m_channel = new_scf(m_sine_stereo_signal, m_pan, m_gain));
    ATTEMPT(b_pan = new_const_signal_mf(0.0));
    ATTEMPT(b_gain = new_const_signal_mf(1.0));
    ATTEMPT(b_channel = new_scf(b_sine_stereo_signal, b_pan, b_gain));
    
    //Create a summer to mix together the two instruments
    ATTEMPT(mixer = new_summer_sf(2));
    summer_sf_assign_input(mixer, 1, m_channel);
    summer_sf_assign_input(mixer, 2, b_channel);

    //Create a final master stereo channel from the mixer output
    ATTEMPT(master_pan = new_const_signal_mf(0.0));
    ATTEMPT(master_gain = new_const_signal_mf(1.0)); 
    ATTEMPT(master_channel = new_scf(mixer, master_pan, master_gain));
    
    //Convert the float channel into an i16 signal 
    ATTEMPT(i16_signal = new_sssi16_from_scf(master_channel));
    
    //Wrap the i16 signal in an i16 channel
    ATTEMPT(i16_channel = new_sci16(i16_signal));
    
    //Insert some events into the sequencers
    float t = 0;
    int i;

    for(i = 0; i < 16; i++) {
    
        sequencer_add_event(m_sequencer, t, NOTE_C4, SEQ_ACTON);
        sequencer_add_event(m_sequencer, t + 200, NOTE_E4, SEQ_ACTON);
        sequencer_add_event(m_sequencer, t + 400, NOTE_G4, SEQ_ACTON);
        t += 600;
    }

    sequencer_add_event(m_sequencer, t, NOTE_G4, SEQ_ACTOFF);
    
    sequencer_add_event(b_sequencer, 0, NOTE_E3, SEQ_ACTON);
    sequencer_add_event(b_sequencer, 2400, NOTE_C3, SEQ_ACTON);
    sequencer_add_event(b_sequencer, 4800, NOTE_D3, SEQ_ACTON);
    sequencer_add_event(b_sequencer, 7200, NOTE_A2, SEQ_ACTON);
    sequencer_add_event(b_sequencer, 9600, NOTE_A2, SEQ_ACTOFF);

    //Finally, stream the i16 channel out to WAV
    write_wav_pcm16_stereo("untitled.wav", i16_channel);
    
    return 0;
}
