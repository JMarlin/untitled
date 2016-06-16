#include <stdio.h>
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
#include "instruments/string.h"

#define BETA 100.0

int main(int argc, char* argv[]) {
    
    Sequencer* snare_sequencer = new_sequencer();
    Sequencer* kick_sequencer = new_sequencer();
    Sequencer* hh_sequencer = new_sequencer();
    Sequencer* string_sequencer = new_sequencer();

    StereoChannel_f* kick_channel = new_scf(
        new_sssf_from_ssmf(
            new_kick(new_cv_from_sequencer(kick_sequencer))
        ),
        new_const_signal_mf(0.0),
        new_const_signal_mf(1.0)
    );

    StereoChannel_f* snare_channel = new_scf(
        new_sssf_from_ssmf(
            new_snare(new_cv_from_sequencer(snare_sequencer))
        ),
        new_const_signal_mf(0.0),
        new_const_signal_mf(0.5)
    );

    StereoChannel_f* hh_channel = new_scf(
        new_sssf_from_ssmf(
            new_envelope_white(new_cv_from_sequencer(hh_sequencer))
        ),
        new_const_signal_mf(-0.125),
        new_const_signal_mf(-0.75)
    );

    StereoChannel_f* string_channel = new_scf(
        new_sssf_from_ssmf(
            new_string(new_cv_from_sequencer(string_sequencer))
            //new_saw_vco(new_cv_from_sequencer(string_sequencer))
        ),
        new_const_signal_mf(0.0),
        new_const_signal_mf(0.0)
    );

    SignalSourceStereo_f* master_summer = new_summer_sf(4);
    summer_sf_assign_input(master_summer, 1, snare_channel);
    summer_sf_assign_input(master_summer, 2, kick_channel);
    summer_sf_assign_input(master_summer, 3, hh_channel);
    summer_sf_assign_input(master_summer, 4, string_channel);
    
    StereoChannel_i16* i16_channel = new_sci16(
        new_sssi16_from_scf(
            new_scf(
                master_summer,
                new_const_signal_mf(0.0),
                new_const_signal_mf(1.0)
            )
        )
    );

    int i, j;

    //120 BPM 4/4, 1 quarter note = 500ms
    //1 eighth note = 250ms
    //2 bars of 4 quarter notes = 500 * 2 * 4 = 4000ms

    float arpeggio[] = {
        NOTE_BF3,
        NOTE_C4,
        NOTE_D4,
        NOTE_EF4,
        NOTE_F4,
        NOTE_G4,
        NOTE_A4,
        NOTE_BF4
    };

    for(i = 0; i < 4; i++) {
      
        //quarter-note string arpeggio
        for(j = 0; j < 8; j++) {

            sequencer_add_event(string_sequencer, (i * 4000) + (j * 500) - 100, arpeggio[j], SEQ_ACTON);
            sequencer_add_event(string_sequencer, (i * 4000) + (j * 500) + 320, arpeggio[j], SEQ_ACTOFF);
        }
  
        //16th-note high-hat
        for(j = 0; j < 32; j++) {
     
            sequencer_add_event(hh_sequencer, (i * 4000) + (j * 125), NOTE_C3, SEQ_ACTON);
            sequencer_add_event(hh_sequencer, (i * 4000) + (j * 125) + 123, NOTE_C3, SEQ_ACTOFF);
        }

        //Snare on 2s and 4s
        sequencer_add_event(snare_sequencer, (i * 4000) + 500, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(snare_sequencer, (i * 4000) + 900, NOTE_C3, SEQ_ACTOFF); 
        sequencer_add_event(snare_sequencer, (i * 4000) + 1500, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(snare_sequencer, (i * 4000) + 1900, NOTE_C3, SEQ_ACTOFF); 
        sequencer_add_event(snare_sequencer, (i * 4000) + 2500, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(snare_sequencer, (i * 4000) + 2900, NOTE_C3, SEQ_ACTOFF);
        sequencer_add_event(snare_sequencer, (i * 4000) + 3500, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(snare_sequencer, (i * 4000) + 3900, NOTE_C3, SEQ_ACTOFF);

        sequencer_add_event(kick_sequencer, (i * 4000), NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 240, NOTE_C3, SEQ_ACTOFF);
        sequencer_add_event(kick_sequencer, (i * 4000) + 250, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 490, NOTE_C3, SEQ_ACTOFF);
        sequencer_add_event(kick_sequencer, (i * 4000) + 1250, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 1490, NOTE_C3, SEQ_ACTOFF);
        sequencer_add_event(kick_sequencer, (i * 4000) + 1625, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 1865, NOTE_C3, SEQ_ACTOFF);
        sequencer_add_event(kick_sequencer, (i * 4000) + 2250, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 2490, NOTE_C3, SEQ_ACTOFF);        
        sequencer_add_event(kick_sequencer, (i * 4000) + 2875, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 3115, NOTE_C3, SEQ_ACTOFF);        
        sequencer_add_event(kick_sequencer, (i * 4000) + 3250, NOTE_C3, SEQ_ACTON);
        sequencer_add_event(kick_sequencer, (i * 4000) + 3490, NOTE_C3, SEQ_ACTOFF);        
    }

    sequencer_add_event(string_sequencer, 16000.0, NOTE_C3, SEQ_ACTEND);

    write_wav_pcm16_stereo("untitled.wav", i16_channel); 
   
    return 0;
}
