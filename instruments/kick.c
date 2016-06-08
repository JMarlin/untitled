#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/white.h"
#include "../devices/adsr.h"
#include "../devices/split.h"
#include "../devices/sine.h"
#include "../devices/vca.h"
#include "../devices/adder.h"
#include "../devices/multiplier.h"

SignalSourceMono_f* new_kick(ControlVoltage* control_voltage) {

    SignalSourceMono_f *env_1, *env_2, *env_3, *env_4;

    new_mf_split(
        new_adsr(
            control_voltage,
            new_const_signal_mf(15.0),
            new_const_signal_mf(20.0),
            new_const_signal_mf(0.0),
            new_const_signal_mf(150.0)
        ),
        &env_1,
        &env_2
    );

    new_mf_split(
        env_2,
        &env_3,
        &env_4
    );

    return new_vca(
        new_adder(
            new_sine_vco(
                new_cv_from_ssmf(
                    new_adder(
                        new_const_signal_mf(60.0),
                        new_multiplier(
                            env_1,
                            new_const_signal_mf(4.0)
                        )
                    ),
                    new_const_signal_mf(1.0)
                )
            ),
            new_multiplier(new_sine_vco(
                new_cv_from_ssmf(
                    new_adder(
                        new_const_signal_mf(130),
                        new_multiplier(
                            env_3,
                            new_const_signal_mf(10.0)
                        ) 
                    ),
                    new_const_signal_mf(1.0)
                )
            ), new_const_signal_mf(0.3333))
        ),
        env_4
    );
}
