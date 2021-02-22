/*
 * spectrum_tools.h
 *
 *  Created on: Feb 7, 2021
 *      Author: mrtbuntu
 */

#ifndef EQUALIZER_H_
#define EQUALIZER_H_
#include <stdlib.h>
#include <stdbool.h>
#include <arm_math.h>
#include <arm_const_structs.h>

#define NUM_SAMPLES 2304

typedef struct eq_config_{
	int cant_filters;
	int num_taps;
	float32_t *filters;
	int16_t *filter_gains;

}eq_config;

void init_equalizer(void);
void equalize_frame(int16_t *input16, int16_t *output16);
void equalizer_change_effect(int16_t *presets);
void recalculate_filter(void);
#endif /* EQUALIZER_H_ */
