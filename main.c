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
#include "devices/vca.h"
#include "devices/freqtocv.h"
#include "devices/adder.h"
#include "devices/multiplier.h"
#include "devices/delay.h"
#include "config.h"
#include "instruments/envelope_white.h"

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
    
    Sequencer* snare_sequencer = new_sequencer();
    
    StereoChannel_f* bass_channel = new_scf(
        new_sssf_from_ssmf(
            new_vca(
                new_fixed_sine(110, 10000.0),
                new_adder(
                    new_const_signal_mf(0.25, 10000.0),
                    new_multiplier(
                        new_const_signal_mf(0.875, 10000.0),
                        new_fixed_sine(53, 10000.0)
                    )
                )
            )
        ),
        new_const_signal_mf(0.0, 10000.0),
        new_const_signal_mf(0.4, 10000.0)
    );
    
    StereoChannel_f* snare_channel = new_scf(
        new_sssf_from_ssmf(
            new_delay(
                new_envelope_white(
                    new_cv_from_sequencer(snare_sequencer)
                ),
                new_multiplier(
                    new_fixed_sine(0.4, 10000.0),
                    new_const_signal_mf(0.3, 10000.0)
                ),
                new_const_signal_mf(0.7, 10000.0),
                new_const_signal_mf(0.0, 10000.0),
                500.0
            )
        ),
        new_const_signal_mf(0.0, 10000.0),
        new_const_signal_mf(1.0, 10000.0)
    );

    SignalSourceStereo_f* master_summer = new_summer_sf(2);
    summer_sf_assign_input(master_summer, 1, bass_channel);
    summer_sf_assign_input(master_summer, 2, snare_channel);

    StereoChannel_i16* i16_channel = new_sci16(
        new_sssi16_from_scf(
            new_scf(
                master_summer,
                new_const_signal_mf(0.0, 10000.0),
                new_const_signal_mf(1.0, 10000.0)
            )
        )
    );
 
    int i;    

    for(i = 0; i < 16; i++) {
    
        sequencer_add_event(snare_sequencer, i*1000.0, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(snare_sequencer, (i*1000.0)+40.0, NOTE_C3, SEQ_ACTOFF);
    }

    sequencer_add_event(snare_sequencer, 10000.0, NOTE_C3, SEQ_ACTEND);

    write_wav_pcm16_stereo("untitled.wav", i16_channel); 
   
    return 0;
}
