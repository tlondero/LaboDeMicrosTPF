/***************************************************************************//**
 @file fft.c
 @brief performs the FFT algorithm with the DSP library funcionts
 @author G. Lambertucci, T. Londero M. Rodriguez, A. Melachupa
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <fft.h>
#include "arm_const_structs.h"
#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BINES 8
#define SIZE 4096

#define BIN8 168513740U
#define BIN7 56851374U
#define BIN6 4851374U
#define BIN5 2300000U
#define BIN4 1638400U
#define BIN3  909600U
#define BIN2  409600U
#define BIN1  209600U

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static arm_cfft_instance_f32* fftSizeToInstance(cfft_size_t size);
static uint32_t fftInstanceToSize(arm_cfft_instance_f32 *instance);

void assignBines(float32_t *bines);
/*******************************************************************************
 * VARIABLES  WITH FILE LEVEL SCOPE
 ******************************************************************************/

arm_cfft_instance_f32 *fftInstance;

/*******************************************************************************
 *******************************************************************************
 GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void fftInit(cfft_size_t size) {
	fftInstance = fftSizeToInstance(size);
}

void fft(float32_t *inputF32, float32_t *outputF32, bool doBitReverse) {
	memcpy(outputF32, inputF32,
			2 * fftInstanceToSize(fftInstance) * sizeof(float32_t)); // Copying input array to preserve it.
	arm_cfft_f32(fftInstance, outputF32, false, doBitReverse);
}

void icfft(float32_t *inputF32, float32_t *outputF32, bool doBitReverse) {
	memcpy(outputF32, inputF32,
			fftInstanceToSize(fftInstance) * 2 * sizeof(float32_t)); // Copying input array to preserve it.
	arm_cfft_f32(fftInstance, outputF32, true, doBitReverse);
}

void fftGetMag(float32_t *inputF32, float32_t *outputF32) {
	arm_cmplx_mag_f32(inputF32, outputF32, fftInstanceToSize(fftInstance));
}

void fftMakeBines8(float32_t *src, float32_t *dst) {
	float32_t maxValue=0;
	uint32_t index=0;
	arm_max_f32(src, SIZE, &maxValue, &index);


	for (int j = 0; j < BINES; j++) {
		switch(j){
		case 0:
			arm_max_f32(src, 40, &maxValue, &index);
			break;
		case 1:
			arm_max_f32(src+40, 60, &maxValue, &index);
			break;
		case 2:
			arm_max_f32(src+100, 100, &maxValue, &index);
			break;
		case 3:
			arm_max_f32(src+200, 300, &maxValue, &index);
			break;
		case 4:
			arm_max_f32(src+500, 600, &maxValue, &index);
			break;
		case 5:
			arm_max_f32(src+1100, 800, &maxValue, &index);
			break;
		case 6:
			arm_max_f32(src+1900, 1000, &maxValue, &index);
			break;
		case 7:
			arm_max_f32(src+2900, 1196, &maxValue, &index);
			break;
		default:
			break;
		}
		dst[j] = maxValue;
//		arm_max_f32(src+(SIZE/BINES)*j, (SIZE/BINES), &maxValue, &index);
//		dst[j] = maxValue;
		assignBines(&(dst[j]));
	}

}

/*******************************************************************************
 *******************************************************************************
 LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
arm_cfft_instance_f32* fftSizeToInstance(cfft_size_t size) {
	arm_cfft_instance_f32 *instance = &arm_cfft_sR_f32_len1024;

	switch (size) {
	case CFFT_16:
		instance = &arm_cfft_sR_f32_len16;
		break;
	case CFFT_32:
		instance = &arm_cfft_sR_f32_len32;
		break;
	case CFFT_64:
		instance = &arm_cfft_sR_f32_len64;
		break;
	case CFFT_128:
		instance = &arm_cfft_sR_f32_len128;
		break;
	case CFFT_256:
		instance = &arm_cfft_sR_f32_len256;
		break;
	case CFFT_512:
		instance = &arm_cfft_sR_f32_len512;
		break;
	case CFFT_1024:
		instance = &arm_cfft_sR_f32_len1024;
		break;
	case CFFT_2048:
		instance = &arm_cfft_sR_f32_len2048;
		break;
	case CFFT_4096:
		instance = &arm_cfft_sR_f32_len4096;
		break;
	}

	return instance;
}

uint32_t fftInstanceToSize(arm_cfft_instance_f32 *instance) {
	uint32_t size = 1024;

	if (instance == &arm_cfft_sR_f32_len16)
		size = 16;
	else if (instance == &arm_cfft_sR_f32_len32)
		size = 32;
	else if (instance == &arm_cfft_sR_f32_len64)
		size = 64;
	else if (instance == &arm_cfft_sR_f32_len128)
		size = 128;
	else if (instance == &arm_cfft_sR_f32_len256)
		size = 256;
	else if (instance == &arm_cfft_sR_f32_len512)
		size = 512;
	else if (instance == &arm_cfft_sR_f32_len1024)
		size = 1024;
	else if (instance == &arm_cfft_sR_f32_len2048)
		size = 2048;
	else if (instance == &arm_cfft_sR_f32_len4096)
		size = 4096;

	return size;
}
void assignBines(float32_t *bines) {
	if(*bines > BIN8){
		*bines=8;
	}
	else if(*bines > BIN7){
		*bines=7;
	}
	else if(*bines > BIN6){
		*bines=6;
	}
	else if(*bines > BIN5){
		*bines=5;
	}
	else if(*bines > BIN4){
		*bines=4;
	}
	else if(*bines > BIN3){
		*bines=3;
	}
	else if(*bines > BIN2){
		*bines=2;
	}
	else if(*bines > BIN1){
		*bines=1;
	}
	else{
		*bines=0;
	}

}
