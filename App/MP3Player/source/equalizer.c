/*
 * spectrum_tools.c
 *
 *  Created on: Feb 7, 2021
 *      Author: MAGT
 */
#include <equalizer.h>
#include <arm_const_structs.h>
#include <arm_math.h>
#include "filters_coefs_101_t.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
static eq_config config;

/* Length of the overall data in the test */
#define SAMPLE_LENGTH 2304

/* Block size for the underlying processing */
#define BLOCKSIZE 32

/* -------------------------------------------------------------------
 * Declare State buffer of size (numTaps + blockSize - 1)
 * ------------------------------------------------------------------- */
//static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
//static uint32_t blockSize = BLOCK_SIZE;
//static uint32_t numBlocks = NUM_SAMPLES / BLOCK_SIZE;
//static arm_fir_instance_f32 S;
//static float32_t *inputF32, *outputF32;
/*******FIR filtering************/

/*********FIR assembly************/
static float32_t filter[NUM_TAPS];
static float32_t temp_filter[NUM_TAPS]; /*"32 - 64 - 125 - 250 - 500 - 1000 - 2000 - 4000 - 8000 - 16000"*/

//static float32_t gain[EQ_NUM_OF_FILTERS] = { -5000.0f, -5000.0f, -5000.0f, -5000.0f, -500.0f, 0.0f, 0.0f, 0.0f };
//static float32_t gain[EQ_NUM_OF_FILTERS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
static int16_t gain_off[EQ_NUM_OF_FILTERS] = { 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0};

//static float32_t gain[EQ_NUM_OF_FILTERS] = { -3.0f, -3.0f, -3.0f, -3.0f, 0.0f, 0.0f, 0.0f, 0.0f };
//static float32_t gain[EQ_NUM_OF_FILTERS] = { -400.0f, -400.0f, -400.0f, -400.0f, -400.0f, 0, 0.f, 0.0f, 0.0f };
//static float32_t gain[EQ_NUM_OF_FILTERS] = { -79, 96, 96, 55, 16, -39, -79, -103, -111, -111 };
//static float32_t gain[EQ_NUM_OF_FILTERS] = { -96, -96, -96, -39, 24, 111, 159, 159, 159, 167 };
//static float32_t gain[EQ_NUM_OF_FILTERS] = { 0, .0, 0, 0, 24, 111, 500, 500, 500, 500 };
/* ----------------------------------------------------------------------
 ** Desired gains, in dB, per band
 ** ------------------------------------------------------------------- */
void init_equalizer(void) {
	config.cant_filters = EQ_NUM_OF_FILTERS;
	config.filters = (float32_t *)eqFirCoeffs32;
	config.filter_gains = gain_off;
	/* Call FIR init function to initialize the instance structure. */
	/*	arm_fir_init_f32(&S, NUM_TAPS, (float32_t*) &filter[0], &firStateF32[0],
	 blockSize);*/
	recalculate_filter();
}
void recalculate_filter(void) {
	/*Clean filter*/
	arm_fill_f32(0.0, filter, NUM_TAPS);
	int i;
	for (i = 0; i < EQ_NUM_OF_FILTERS; i++) {
		/*Copy filter coefficients to a temporal vector*/
		arm_copy_f32(&config.filters[NUM_TAPS * i], temp_filter, NUM_TAPS);
		/*Generate gain vector*/
		float32_t dummy2 =(float32_t)config.filter_gains[i];
		float32_t dummy = (pow(10, (dummy2/ 100.0f)));
		arm_scale_f32(temp_filter, dummy, temp_filter, NUM_TAPS);
		/*Add the filter to the end result*/
		arm_add_f32(filter, temp_filter, filter, NUM_TAPS);
	}
}

void equalizer_change_effect(int16_t *presets){
	config.filter_gains = presets;
	recalculate_filter();
}

static float32_t inputF32[NUM_SAMPLES];
static float32_t outputF32[NUM_SAMPLES + NUM_TAPS - 1];
void int16_to_f32(int16_t *input16, float32_t *input32, int num) {
	int i = 0;
	for (i = 0; i < num; i++) {
		inputF32[i] = (float32_t) (input16[i]);
	}
}
/* Initialize the state and coefficient buffers for all Biquad sections */

void f32_to_int16(float32_t *outputF32, int16_t *output16, int num) {
	int i = 0;
	/*for (i = 0; i < num; i++) {
	 output16[i] = (int16_t) (outputF32[i] * (16384.0f));
	 }*/
	float32_t max_value = -FLT_MIN;
	for (i = 0; i < num; i++) {
		float temp = outputF32[i];
		arm_abs_f32(&temp, &temp, 1);
		if (temp > max_value) {
			max_value = temp;
		}
	}
	arm_scale_f32(outputF32, (SHRT_MAX / max_value)*0.5f, outputF32, NUM_SAMPLES);

	for (i = 0; i < NUM_SAMPLES; i++) {
		output16[i] = (int16_t) outputF32[i];
	}
}

void equalize_frame(int16_t *input16, int16_t *output16) {

	int16_to_f32(input16, inputF32, NUM_SAMPLES);
	arm_conv_f32(inputF32, NUM_SAMPLES, filter, NUM_TAPS, outputF32);
	f32_to_int16(outputF32 + 50, output16, NUM_SAMPLES);
}

