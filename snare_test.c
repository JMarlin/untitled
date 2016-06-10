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
#include "devices/adder.h"
#include "devices/multiplier.h"
#include "devices/delay.h"
#include "devices/adsr.h"
#include "devices/saw.h"
#include "instruments/kick.h"
#include "config.h"
#include "instruments/foot_hh.h"
#include "instruments/envelope_sine.h"
#include "instruments/snare.h"

#define BETA 100.0

int main(int argc, char* argv[]) {
    
    Sequencer* snare_sequencer = new_sequencer();

    StereoChannel_f* snare_channel = new_scf(
        new_sssf_from_ssmf(
            new_snare(new_cv_from_sequencer(snare_sequencer))
        ),
        new_const_signal_mf(0.0),
        new_const_signal_mf(1.0)
    );

    SignalSourceStereo_f* master_summer = new_summer_sf(1);
    summer_sf_assign_input(master_summer, 1, snare_channel);

    StereoChannel_i16* i16_channel = new_sci16(
        new_sssi16_from_scf(
            new_scf(
                master_summer,
                new_const_signal_mf(0.0),
                new_const_signal_mf(1.0)
            )
        )
    );

    for(i = 0; i < 32; i++) {
        
        sequencer_add_event(snare_sequencer, i*800, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(snare_sequencer, (i*800)+30, NOTE_C3, SEQ_ACTOFF); 
    }

    sequencer_add_event(snare_sequencer, 32000.0, NOTE_C3, SEQ_ACTEND);

    write_wav_pcm16_stereo("untitled.wav", i16_channel); 
   
    return 0;
}
