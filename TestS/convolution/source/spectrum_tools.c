/*
 * spectrum_tools.c
 *
 *  Created on: Feb 7, 2021
 *      Author: mrtbuntu
 */
#include <spectrum_tools.h>
#include <filters_coefs.h>
#include <stdio.h>
// Init equalizer data
static float32_t *fft_input_buffer;
static float32_t *fft_output_buffer;
//static float32_t *filters_fft;
static eq_config *config;

//Init FFT data
static arm_status status;                           /* Status of the example */

/* CFFT Structure instance pointer */
static arm_cfft_instance_f32 *cfft_instance_ptr = &arm_cfft_sR_f32_len2048;//&arm_cfft_sR_f32_len64;
    //(arm_cfft_instance_f32*) &cfft_instance;


static float32_t filters_fft[EQ_NUM_OF_FILTERS][2*FFT_LEN];





/*******FIR filtering***********/
#define BLOCK_SIZE 32
/* -------------------------------------------------------------------
 * Declare Test output buffer
 * ------------------------------------------------------------------- */
static float32_t testOutput[NUM_SAMPLES];
/* -------------------------------------------------------------------
 * Declare State buffer of size (numTaps + blockSize - 1)
 * ------------------------------------------------------------------- */
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];

static uint32_t blockSize = BLOCK_SIZE;
static uint32_t numBlocks = NUM_SAMPLES/BLOCK_SIZE;
static arm_fir_instance_f32 S;
static arm_status status;
static float32_t  *inputF32, *outputF32;

/*******FIR filtering************/


/*********FIR assembly************/
static float32_t filter[NUM_TAPS];
static float32_t temp_filter[NUM_TAPS];
static float32_t gain[NUM_TAPS];
/*********FIR assembly************/


bool init_equalizer(eq_config *config_ptr){

	config = config_ptr;
	/* Call FIR init function to initialize the instance structure. */
	arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&filter[0], &firStateF32[0], blockSize);
	recalculate_filter();

	//int filter_index = 0;
	/*for(filter_index=0; filter_index<config->cant_filters; filter_index++){ //
		/* Copy the input values to the fft input buffers */
	//	  arm_copy_f32(&config->filters[filter_index*config->num_taps],  &filters_fft[filter_index], config->num_taps);
		/* Transform input filter from time domain to frequency domain A[k] */
	//	  arm_cfft_f32(cfft_instance_ptr, &filters_fft[filter_index],0,1);
	//}

	//return true;

}


void recalculate_filter(void){
	/*Clean filter*/
	arm_fill_f32(0.0, filter, NUM_TAPS);
	int i;
	for(i=0;i<EQ_NUM_OF_FILTERS;i++){
		/*Copy filter coefficients to a temporal vector*/
		arm_copy_f32(&config->filters[NUM_TAPS*i], temp_filter, NUM_TAPS);
		/*Generate gain vector*/
		arm_fill_f32(config->filter_gains[i], gain, NUM_TAPS);
		/*Apply gain to FIR coeffs*/
		arm_mult_f32(temp_filter, gain, temp_filter, NUM_TAPS);
		/*Add the filter to the end result*/
		arm_add_f32(filter, temp_filter, filter, NUM_TAPS);
	}


}


void equalize_frame(float *inputF32, float *outputF32){
	int i;
	for(i=0; i < numBlocks; i++)
	  {
		arm_fir_f32(&S, inputF32 + (i * blockSize), outputF32 + (i * blockSize), blockSize);
	  }
}



