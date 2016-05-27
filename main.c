#include "channel/channel.h"
#include "devices/sine.h"
#include "devices/square.h"
#include "devices/controlvoltage.h"
#include "devices/sequencer.h"
#include "devices/pulse.h"
#include "devices/white.h"
#include "devices/sofilter.h"
#include "wavlib/wavlib.h"
#include "devices/sum.h"
#include "devices/freqtocv.h"
#include "devices/adder.h"
#include "devices/multiplier.h"
#include "config.h"
#include "instruments/envelope_sine.h"

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

#define BETA 100.0

int main(int argc, char* argv[]) {
    
    Sequencer* sequencer = new_sequencer();

    StereoChannel_i16* i16_channel = new_sci16(new_sssi16_from_scf(new_scf(
        new_sssf_from_ssmf(
            new_envelope_sine(
                new_cv_from_sequencer(
                    sequencer
                ) 
            )
        ),
        new_const_signal_mf(0.0, 10000.0),
        new_const_signal_mf(0.0, 10000.0)
    )));
 
    int i;    

    for(i = 0; i < 16; i++) {
    
        sequencer_add_event(sequencer, i*400, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(sequencer, (i*400)+200, NOTE_C3, SEQ_ACTOFF);
    }

    write_wav_pcm16_stereo("untitled.wav", i16_channel); 
   
    return 0;
}
