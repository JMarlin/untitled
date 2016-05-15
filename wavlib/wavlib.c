#include "wavlib.h"
#include "../channel/channel.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

int write_wav_pcm16_stereo(char* file_name, StereoChannel_i16* stereo_channel) {
    
    uint32_t audio_size;
    int16_t l_sample, r_sample;
    
    //Stage 1, write a blank header and dump in audio data
    FILE* out_file = fopen(file_name, "wb");

    if(!out_file)
        return 0;
    
    //Write 44 bytes of blank header space
    fwrite((uint16_t []){0}, sizeof(uint16_t), 22, out_file);

    //Stream the input channel into the data section
    for(audio_size = 0; sci16_pull_next_sample(stereo_channel, &l_sample, &r_sample); audio_size += 4) {
        
        fwrite(&l_sample, sizeof(int16_t), 1, out_file);
        fwrite(&r_sample, sizeof(int16_t), 1, out_file);
    }

    //Rewind the file, start stage 2: writing the properly calculated header
    fseek(out_file, 0, SEEK_SET);

    uint32_t riff_size = audio_size + 36; //data size + header size - 8 for the two fields at the top that aren't counted
	               
    //Need to failure check this stuff
    //Write RIFF header chunk
    fprintf(out_file, "RIFF");
    fwrite(&riff_size, sizeof(uint32_t), 1, out_file);
    fprintf(out_file, "WAVE");
    
    //Write sub chunk 1, PCM format info
    fprintf(out_file, "fmt ");
    fwrite((uint32_t []){16}, sizeof(uint32_t, 1, out_file); //PCM header is 16 bytes
    fwrite((uint16_t []){1}, sizeof(uint16_t), 1, out_file); //Audio Format 1, PCM
    fwrite((uint16_t []){2}, sizeof(uint16_t), 1, out_file); //2 channels
    fwrite((uint32_t []){44100}, sizeof(uint32_t), 1, out_file); //44.1khz sample rate
    fwrite((uint32_t []){176400}, sizeof(uint32_t), 1, out_file); //Byte rate
    fwrite((uint16_t []){4}, sizeof(uint16_t), 1, out_file); //Bytes per sample, combined channels
    fwrite((uint16_t []){16}, sizeof(uint16_t), 1, out_file); //Bits per sample

    //Write sub chunk 2, PCM data
    fprintf(out_file, "data");
    fwrite(&audio_size, sizeof(uint32_t), 1, out_file);

    //File is finished, close
    fclose(out_file);

    return 1;
}

