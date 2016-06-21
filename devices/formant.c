#include "../channel/channel.h"
#include "formant.h"
#include "../util.h"
#include <stdlib.h>

const double coeff[5][11]= {
  { 
      8.11044e-06, 8.943665402, -36.83889529, 92.01697887,
      -154.337906, 181.6233289, -151.8651235, 89.09614114, 
      -35.10298511, 8.388101016, -0.923313471 ///A
  },
  {
      4.36215e-06, 8.90438318, -36.55179099, 91.05750846,
      -152.422234, 179.1170248, -149.6496211, 87.78352223,
      -34.60687431, 8.282228154, -0.914150747 ///E
  },
  {
      3.33819e-06, 8.893102966, -36.49532826, 90.96543286,
      -152.4545478, 179.4835618, -150.315433, 88.43409371,
      -34.98612086, 8.407803364, -0.932568035 ///I
  },
  {
      1.13572e-06, 8.994734087, -37.2084849, 93.22900521,
      -156.6929844, 184.596544, -154.3755513, 90.49663749,
      -35.58964535, 8.478996281, -0.929252233 ///O
  },
  {
      4.09431e-07, 8.997322763, -37.20218544, 93.11385476,
      -156.2530937, 183.7080141, -153.2631681, 89.59539726,
      -35.12454591, 8.338655623, -0.910251753 ///U
  }
}; 

typedef struct FormantFilterGeneratorEnv_s {
    SignalSourceMono_f* input_signal;
    int vowelnum;
    double memory[10];
} FormantFilterGeneratorEnv;

int formant_filter_generator(float* sample, void* environment) {

    FormantFilterGeneratorEnv* vars = (FormantFilterGeneratorEnv*)environment;

    if(!ssmf_pull_next_sample(vars->input_signal, &input_sample))
        return 0;

    sample[0] = (float)( coeff[vars->vowelnum][0]  * input_sample +
                         coeff[vars->vowelnum][1]  * vars->memory[0] +  
                         coeff[vars->vowelnum][2]  * vars->memory[1] +
                         coeff[vars->vowelnum][3]  * vars->memory[2] +
                         coeff[vars->vowelnum][4]  * vars->memory[3] +
                         coeff[vars->vowelnum][5]  * vars->memory[4] +
                         coeff[vars->vowelnum][6]  * vars->memory[5] +
                         coeff[vars->vowelnum][7]  * vars->memory[6] +
                         coeff[vars->vowelnum][8]  * vars->memory[7] +
                         coeff[vars->vowelnum][9]  * vars->memory[8] +
                         coeff[vars->vowelnum][10] * vars->memory[9] );

    vars->memory[9]= vars->memory[8];
    vars->memory[8]= vars->memory[7];
    vars->memory[7]= vars->memory[6];
    vars->memory[6]= vars->memory[5];
    vars->memory[5]= vars->memory[4];
    vars->memory[4]= vars->memory[3];
    vars->memory[3]= vars->memory[2];
    vars->memory[2]= vars->memory[1];                     
    vars->memory[1]= vars->memory[0];
    vars->memory[0]= (double)sample[0];

    return 1;
}

int formant_filter_deleter(void* environment) {

    FormantFilterGeneratorEnv* vars = (FormantFilterGeneratorEnv*)environment;

    //delete_ssmf(vars->input_signal);
    return 1;
}

SignalSourceMono_f* new_formant_filter(SignalSourceMono_f* input_signal, int vowelnum) {

    FormantFilterGeneratorEnv* environment = new(FormantFilterGeneratorEnv);
    
    if(!environment) {
    
        formant_filter_deleter(environment);
        return (SignalSourceMono_f*)0;
    }
    
    memset(&environment->memory[0], 0, 10 * sizeof(double));
    environment->vowelnum = vowelnum;
    environment->input_signal = input_signal;
    
    SignalSourceMono_f* out_signal = new_ssmf(formant_filter_generator); //, formant_filter_deleter, environment);
    
    //start delete
    if(!out_signal) 
        return out_signal;
        
    out_signal->environment = environment;
    //end delete
    
    return out_signal;
}
