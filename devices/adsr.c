#include "../channel/channel.h"
#include "adsr.h"

typedef struct ADSRGeneratorEnv {
    ControlVoltage* trigger_cv;
    SignalSourceMono_f* input_signal;
    SignalSourceMono_f* a_signal;
    SignalSourceMono_f* d_signal;
    SignalSourceMono_f* s_signal;
    SignalSourceMOno_f* r_signal;
    float time;
} ADSRGeneratorEnv;

SignalSourceMono_f* new_adsr(SignalSourceMono_f* input_signal, ControlVoltage* trigger_cv, SignalSourceMono_f* a_signal, SignalSourceMono_f* d_signal, SignalSourceMono_f* s_signal, SignalSourceMono_f* r_signal);

#endif //ADSR_H
