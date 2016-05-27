#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/sine.h"
#include "../devices/adsr.h"
#include "../devices/split.h"

SignalSourceMono_f* new_envelope_sine(ControlVoltage* control_voltage) {

    ControlVoltage* cv_copy_1, *cv_copy_2;
    new_split(control_voltage, &cv_copy_1, &cv_copy_2);

    return new_adsr(
        new_sine_vco(cv_copy_1),
        cv_copy_2,
        new_const_signal_mf(10.0, 10000),
        new_const_signal_mf(100.0, 10000),
        new_const_signal_mf(0.25, 10000),
        new_const_signal_mf(250.0, 10000)
    );
}
