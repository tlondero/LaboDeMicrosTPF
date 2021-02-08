/*
 * spectrum_tools.h
 *
 *  Created on: Feb 7, 2021
 *      Author: mrtbuntu
 */

#ifndef SPECTRUM_TOOLS_H_
#define SPECTRUM_TOOLS_H_
#include <stdlib.h>
#include <stdbool.h>
#include <arm_math.h>
#include <arm_const_structs.h>

#define NUM_SAMPLES 2304
#define FFT_LEN 2048

typedef struct eq_config_{
	int cant_filters;
	int num_taps;
	uint16_t fft_len;
	float32_t *filters;
	float32_t *filter_gains;

}eq_config;

void recalculate_filter(void);
#endif /* SPECTRUM_TOOLS_H_ */
