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
#define SIZE 1024
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static arm_cfft_instance_f32 * fftSizeToInstance(cfft_size_t size);
static uint32_t fftInstanceToSize(arm_cfft_instance_f32 * instance);


/*******************************************************************************
 * VARIABLES  WITH FILE LEVEL SCOPE
 ******************************************************************************/

arm_cfft_instance_f32 * fftInstance;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void fftInit(cfft_size_t size)
{
  fftInstance = fftSizeToInstance(size);
}

void fft(float32_t * inputF32, float32_t * outputF32, bool doBitReverse)
{
  memcpy(outputF32, inputF32, 2*fftInstanceToSize(fftInstance) * sizeof(float32_t));    // Copying input array to preserve it.
  arm_cfft_f32(fftInstance, outputF32, false, doBitReverse);
}

void icfft(float32_t * inputF32, float32_t * outputF32, bool doBitReverse)
{
  memcpy(outputF32, inputF32, fftInstanceToSize(fftInstance) * 2 * sizeof(float32_t));    // Copying input array to preserve it.
  arm_cfft_f32(fftInstance, outputF32, true, doBitReverse);
}

void fftGetMag(float32_t * inputF32, float32_t * outputF32)
{
  arm_cmplx_mag_f32(inputF32, outputF32, fftInstanceToSize(fftInstance));
}


void fftMakeBines8(float32_t *src,float32_t * dst) {
	float32_t maxValue=0;
	uint32_t index=0;
	arm_max_f32(src, SIZE, &maxValue, &index);
	for (int j = 0; j < BINES; j++) {
		float32_t aux=0;
		for (int i = 0; i < SIZE/BINES; i++) {
			aux+=src[i+(SIZE/BINES)*j]/maxValue;
			if(i==(SIZE/BINES)-1){
				dst[j]=aux/128;
			}
		}
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
arm_cfft_instance_f32 * fftSizeToInstance(cfft_size_t size)
{
  arm_cfft_instance_f32 * instance = &arm_cfft_sR_f32_len1024;

  switch (size)
  {
  case CFFT_16: instance = &arm_cfft_sR_f32_len16; break;
  case CFFT_32: instance = &arm_cfft_sR_f32_len32; break;
  case CFFT_64: instance = &arm_cfft_sR_f32_len64; break;
  case CFFT_128: instance = &arm_cfft_sR_f32_len128; break;
  case CFFT_256: instance = &arm_cfft_sR_f32_len256; break;
  case CFFT_512: instance = &arm_cfft_sR_f32_len512; break;
  case CFFT_1024: instance = &arm_cfft_sR_f32_len1024; break;
  case CFFT_2048: instance = &arm_cfft_sR_f32_len2048; break;
  case CFFT_4096: instance = &arm_cfft_sR_f32_len4096; break;
  }

  return instance;
}

uint32_t fftInstanceToSize(arm_cfft_instance_f32 * instance)
{
  uint32_t size = 1024;

  if (instance == &arm_cfft_sR_f32_len16) size = 16;
  else if (instance == &arm_cfft_sR_f32_len32) size = 32;
  else if (instance == &arm_cfft_sR_f32_len64) size = 64;
  else if (instance == &arm_cfft_sR_f32_len128) size = 128;
  else if (instance == &arm_cfft_sR_f32_len256) size = 256;
  else if (instance == &arm_cfft_sR_f32_len512) size = 512;
  else if (instance == &arm_cfft_sR_f32_len1024) size = 1024;
  else if (instance == &arm_cfft_sR_f32_len2048) size = 2048;
  else if (instance == &arm_cfft_sR_f32_len4096) size = 4096;

  return size;
}
