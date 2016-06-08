#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/sine.h"
#include "../devices/adsr.h"
#include "../devices/split.h"
#include "../devices/vca.h"
#include "../devices/adder.h"
#include "../devices/saw.h"
#include "../devices/cvmultiplier.h"
#include "../devices/square.h"

SignalSourceMono_f* new_envelope_sine(ControlVoltage* control_voltage) {

    ControlVoltage *cv_copy_1, *cv_copy_2, *cv_copy_3, *cv_copy_4;
    new_cv_split(control_voltage, &cv_copy_1, &cv_copy_2);
    new_cv_split(cv_copy_1, &cv_copy_3, &cv_copy_4);

    return new_vca(
        
        new_adder(
            new_saw_vco(
                new_cv_pitch_multiplier(
                    cv_copy_2,
                    new_const_signal_mf(2.0)
                )
            ),
            new_square_vco(cv_copy_3)
        ),
        new_adsr(
            cv_copy_4,
            new_const_signal_mf(25.0),
            new_const_signal_mf(120.0),
            new_const_signal_mf(0.8),
            new_const_signal_mf(300.0)
        )
    );
}
