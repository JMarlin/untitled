#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/adder.h"
#include "../devices/multiplier.h"
#include "../devices/adsr.h"
#include "../devices/vca.h"
#include "../devices/split.h"
#include "../devices/sine.h"

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
    //new_cv_split(cv_1, &cv_3, &cv_4);

    return new_adder(
        new_add_element(
            cv_1, 
            230.0,
            10.0,
            13.0,
            0.0,
            300.0,
            0.65
        ),
        new_add_element(
            cv_2,
            410.0,
            10.0,
            10.0,
            0.0,
            220.0,
            0.35
        )
    );
}
