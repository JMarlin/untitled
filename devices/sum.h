#ifndef SUM_H
#define SUM_H

#include "../channel/channel.h"

SignalSourceStereo_f* new_summer_sf(unsigned input_count);
void summer_sf_assign_input(SignalSourceStereo_f* summer, unsigned input_number, StereoChannel_f* source);

#endif //SUM_H