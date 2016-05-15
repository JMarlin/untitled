#ifndef WAVLIB_H
#define WAVLIB_H

#include "../channel/channel.h"

int write_wav_pcm16_stereo(char* file_name, StereoChannel_i16* stereo_channel);

#endif //WAVLIB_H
