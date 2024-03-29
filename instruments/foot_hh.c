#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/white.h"
#include "../devices/adsr.h"
#include "../devices/split.h"
#include "../devices/vca.h"

SignalSourceMono_f* new_envelope_white(ControlVoltage* control_voltage) {

    return new_vca(
        new_white_noise(10000.0),
        new_adsr(
            control_voltage,
            new_const_signal_mf(15.0),
            new_const_signal_mf(125.0),
            new_const_signal_mf(-1),
            new_const_signal_mf(0.01)
        )
    );
}
