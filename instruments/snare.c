#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/adder.h"
#include "../devices/multiplier.h"
#include "../devices/adsr.h"
#include "../devices/vca.h"
#include "../devices/split.h"
#include "../devices/sine.h"
#include "../devices/white.h"

SignalSourceMono_f* new_add_element(ControlVoltage* cv, float f, float a, float d, float s, float r, float pct) {

    return new_vca(
        new_fixed_sine(f),
        new_adder(
            new_multiplier(
                new_adsr(
                    cv,
                    new_const_signal_mf(a),
                    new_const_signal_mf(d),
                    new_const_signal_mf(s),
                    new_const_signal_mf(r)
                ),
                new_const_signal_mf(pct)
            ),
            new_const_signal_mf(pct - 1)
        )
    );
}

SignalSourceMono_f* new_snare(ControlVoltage* control_voltage) {

    ControlVoltage *cv_1, *cv_2, *cv_3, *cv_4;
    new_cv_split(control_voltage, &cv_1, &cv_2);
    new_cv_split(cv_1, &cv_3, &cv_4);

    return new_adder(
        new_multiplier(
            new_vca(
                new_white_noise(),
                new_adsr(
                    cv_2,
                    new_const_signal_mf(2),
                    new_const_signal_mf(260),
                    new_const_signal_mf(-1),
                    new_const_signal_mf(0.01)
                )
            ),
            new_const_signal_mf(0.45)
        ),
        new_adder(
            new_add_element(
                cv_3, 
                330.0,
                2.0,
                230.0,
                -1.0,
                0.01,
                0.75
            ),
            new_add_element(
                cv_4,
                180.0,
                1.0,
                210.0,
                -1.0,
                0.01,
                0.25
            )
        )
    );
}
