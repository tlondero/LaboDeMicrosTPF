/*
 * spectrum_tools.c
 *
 *  Created on: Feb 7, 2021
 *      Author: mrtbuntu
 */
#include <equalizer.h>
#include <filters_coefs.h>
#include <stdio.h>
#include <math.h>
static eq_config config;

static arm_status status; /* Status of the example */

/*******FIR filtering***********/
#define BLOCK_SIZE 32

/* -------------------------------------------------------------------
 * Declare State buffer of size (numTaps + blockSize - 1)
 * ------------------------------------------------------------------- */
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];

static uint32_t blockSize = BLOCK_SIZE;
static uint32_t numBlocks = NUM_SAMPLES / BLOCK_SIZE;
static arm_fir_instance_f32 S;
//static float32_t *inputF32, *outputF32;

/*******FIR filtering************/

/*********FIR assembly************/
static float32_t filter[NUM_TAPS];
static float32_t temp_filter[NUM_TAPS];
static float32_t gain[EQ_NUM_OF_FILTERS];
/*********FIR assembly************/

void init_equalizer() {

	config.cant_filters=EQ_NUM_OF_FILTERS;
	config.filters=eqFirCoeffs32;
	config.filter_gains = gain;
	/* Call FIR init function to initialize the instance structure. */
	arm_fir_init_f32(&S, NUM_TAPS, (float32_t*) &filter[0], &firStateF32[0],
			blockSize);
	recalculate_filter();

}

void change_gains(float *filter_gains){
	config.filter_gains = filter_gains;
}

void recalculate_filter(void) {
	/*Clean filter*/
	arm_fill_f32(0.0, filter, NUM_TAPS);
	int i;
	for (i = 0; i < EQ_NUM_OF_FILTERS; i++) {
		/*Copy filter coefficients to a temporal vector*/
		arm_copy_f32(&config.filters[NUM_TAPS * i], temp_filter, NUM_TAPS);
		/*Generate gain vector*/

		float dummy = (pow(10, (config.filter_gains[i] / 10)));
		arm_scale_f32(temp_filter, dummy, temp_filter, NUM_TAPS);
		/*Add the filter to the end result*/
		arm_add_f32(filter, temp_filter, filter, NUM_TAPS);
	}

}

static float32_t backup_input[NUM_SAMPLES];

static float32_t inputF32[NUM_SAMPLES];
static float32_t outputF32[NUM_SAMPLES];

void int16_to_f32(int16_t *input16, float32_t *input32, int num){
	int i = 0;
	for(i=0; i<num; i++){
		inputF32[i] = (float32_t)input16[i];
	}

}

void f32_to_int16(float32_t *outputF32, int16_t *output16, int num){
	int i = 0;
	for(i=0; i<num; i++){
		output16[i] = (int16_t)outputF32[i];
	}

}

void equalize_frame(int16_t *input16, int16_t *output16) {

	int16_to_f32(input16, inputF32, NUM_SAMPLES);

	int i;
	arm_copy_f32(inputF32, backup_input, NUM_SAMPLES);
	for (i = 0; i < numBlocks; i++) {
		arm_fir_f32(&S, backup_input + (i * blockSize), outputF32 + (i * blockSize),
				blockSize);
	}

	f32_to_int16(outputF32, output16, NUM_SAMPLES);
}

