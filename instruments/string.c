#include "../channel/channel.h"
#include "../devices/controlvoltage.h"
#include "../devices/saw.h"
#include "../devices/vca.h"
#include "../devices/adsr.h"
#include "../devices/delay.h"
#include "../devices/adder.h"
#include "../devices/multiplier.h"

//A patch to attempt simulation of a traditional string instrument using
//delay to model the instrument's internal resonances in three dimensions

SignalSourceMono_f* new_string(ControlVoltage* control_voltage) {

    ControlVoltage *cv_temp_1, *cv_temp_2, *cv_1, *cv_2, *cv_3, *cv_4;

    new_cv_split(control_voltage, &cv_temp_1, &cv_temp_2);
    new_cv_split(cv_temp_1, &cv_1, &cv_2);
    new_cv_split(cv_temp_2, &cv_3, &cv_4);

    return new_vca(
        new_adder(
            new_multiplier(
                new_delay(
                    new_saw_vco(cv_1),
                    new_const_signal_mf(1.0), //Delay for 100% of the specified delay time
                    new_const_signal_mf(0.7), //No feedback/only one repeat
                    new_const_signal_mf(0.0), //Delayed signal only
                    0.2352 //Delay from front of instrument to back
                ),
                new_const_signal_mf(0.333) //This signal is only 1.3 of the total
            ),
            new_adder(
                new_multiplier(
                     new_delay(
                         new_saw_vco(cv_2),
                         new_const_signal_mf(1.0), 
                         new_const_signal_mf(0.67),
                         new_const_signal_mf(0.0),
                         0.8823 //Delay from one side of instrument to the other
                     ),
                     new_const_signal_mf(0.333) //This signal is only 1/3 of the total
                ),
                new_multiplier(
                    new_delay(
                        new_saw_vco(cv_3),
                        new_const_signal_mf(1.0),
                        new_const_signal_mf(0.65),
                        new_const_signal_mf(0.0),
                        1.3529 //Delay from neck to butt of instrument
                    ),
                    new_const_signal_mf(0.333) //This signal is only 1/3 of the total
                )
            )
        ),
        new_adsr( //Maybe add a bit of enveolope-controlled LFO tremolo here later
            cv_4,
            new_const_signal_mf(170.0),
            new_const_signal_mf(1.0),
            new_const_signal_mf(1.0),
            new_const_signal_mf(400.0)
        )
    );
}
