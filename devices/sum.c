#include "../channel/channel.h"

typedef struct SummerSFGeneratorEnv {
    unsigned input_count;
    StereoChannel_f** input;
} SummerSFGeneratorEnv;

int summer_sf_generator(float* l_sample, float* r_sample, void* environment) {
    
    SummerSFGeneratorEnv* vars = (SummerSFGeneratorEnv)environment;
    
    float temp_sample_l, temp_sample_r;
    int running = 0;
    
    l_sample[0] = 0;
    r_sample[0] = 0;
    
    for(var i = 0; i < vars->input_count; i++) {
        
        if(vars->input[i]) {
            
            if(scf_pull_next_sample(vars->input[i], &temp_sample_l, &temp_sample_r))
                running = 1;
                
            l_sample[0] += temp_sample_l;
            r_sample[0] += temp_sample_r;
        }
    }
    
    return running;
}

SignalSourceStereo_f* new_summer_sf(unsigned input_count) {
    
    if(input_count == 0)
        return (SignalSourceStereo_f*)0;
    
    SignalSourceStereo_f* out_signal = new_sssf(summer_sf_generator);
    
    if(!out_signal)
        return out_signal;
        
    SummerSFGeneratorEnv* environment = (SummerSFGeneratorEnv*)malloc(sizeof(SummerSFGeneratorEnv));
    
    if(!environment) {
        
        //delete_sssf(summer_sf_generator);
        return (SignalSourceStereo_f*)0;
    }
    
    environment->input = (StereoChannel_f**)malloc(input_count * sizeof(StereoChannel_f*));
    
    if(!environment->input) {
        
        //delete_sssf(summer_sf_generator);
        free(environment);
        return (SignalSourceStereo_f*)0;
    }
    
    environment->input_count = input_count;
    
    for(int i = 0; i < input_count; i++)
        environment->input[i] = (StereoChannel_f*)0;
        
    out_signal->environment = environment;
    
    return out_signal;    
}

//Note that this numbering is 1-based
void summer_sf_assign_input(SignalSourceStereo_f summer, unsigned input_number, StereoChannel_f* source) {
    
    if(!summer)
        return;
    
    SummerSFGeneratorEnv* vars = (SummerSFGeneratorEnv*)summer->environment;
    
    if(input_number == 0 || input_number > vars->input_count)
        return;
        
    vars->input[input_number-1] =  source;
}
