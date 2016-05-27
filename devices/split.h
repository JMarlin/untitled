#ifndef SPLIT_H
#define SPLIT_H

#include "../channel/channel.h"
#include "controlvoltage.h"

void new_split(ControlVoltage* input_cv, ControlVoltage** out_cv_a, ControlVoltage** out_cv_b);

#endif //SPLIT_H
