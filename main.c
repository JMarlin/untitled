#include <stdio.h>
#include "channel/channel.h"
#include "devices/sine.h"
#include "devices/square.h"
#include "devices/controlvoltage.h"
#include "devices/sequencer.h"
#include "devices/pulse.h"
#include "devices/white.h"
#include "devices/bqfilter.h"
#include "wavlib/wavlib.h"
#include "devices/sum.h"
#include "devices/vca.h"
#include "devices/adder.h"
#include "devices/multiplier.h"
#include "devices/delay.h"
#include "devices/adsr.h"
#include "devices/saw.h"
#include "instruments/kick.h"
#include "devices/bqfilter.h"
#include "config.h"
#include "instruments/foot_hh.h"
#include "instruments/envelope_sine.h"
#include "instruments/snare.h"
#include "instruments/string.h"

#define BETA 100.0

int main(int argc, char* argv[]) {

    Sequencer* test_sequencer = new_sequencer();

    StereoChannel_i16* i16_channel = new_sci16(
        new_sssi16_from_scf(
            new_scf(
                new_sssf_from_ssmf(
                    new_vca(
                        new_bq_filter(
                            new_white_noise(),
                            0.3044132683847495,
                            -0.27723766055640886,
                            0.09808412473565896,
                            0.5448028485418657,
                            0.22453790221868294
                        ),
                        new_adsr(
                            new_cv_from_sequencer(test_sequencer),
                            new_const_signal_mf(0.01),
                            new_const_signal_mf(0.01),
                            new_const_signal_mf(1.0),
                            new_const_signal_mf(0.01)
                        )
                    )
                ),
                new_const_signal_mf(0.0),
                new_const_signal_mf(1.0)
            )
        )
    );

    sequencer_add_event(test_sequencer, 0, NOTE_C3, SEQ_ACTON);
    sequencer_add_event(test_sequencer, 4000.0, NOTE_C3, SEQ_ACTOFF);
    sequencer_add_event(test_sequencer, 4000.0, NOTE_C3, SEQ_ACTEND);

    write_wav_pcm16_stereo("untitled.wav", i16_channel); 
   
    return 0;
}
