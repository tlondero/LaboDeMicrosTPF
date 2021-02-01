/***************************************************************************/ /**
 @file     equaliser.c
 @brief    ...
 @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Equaliser.h"
#include "arm_math.h"
#include "math_helper.h"
#include <stdio.h>
#include <stdlib.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_BLOCKSIZE 2048
#define FRAME_SIZE 2304
#define NFFT_MAGT 4096


/*******************************************************************************
 *  VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint32_t eqFrameSize;
//static uint32_t blockSize = BLOCK_SIZE;

//static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1]; // State of the FIR filter. Needed for initialisation.

arm_fir_instance_f32 parallelFilter[EQ_NUM_OF_FILTERS]; // EQ filter implemented as parallel bandpass FIR filters.



static float32_t eqGains32[EQ_NUM_OF_FILTERS] = // Multiplier of each equaliser (EQ_NUM_OF_FILTERS bands).
	{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};



/*******************************************************************************
 *******************************************************************************
 GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static float32_t outputAux[NFFT_MAGT];
static float32_t inputAux[NFFT_MAGT];
static float32_t filtFFTAuxIN[NFFT_MAGT];
static float32_t filtFFTAuxOUT[NFFT_MAGT];
static float32_t filtFFTAuxCUMSUM[NFFT_MAGT];

void eqInit(uint32_t frameSize)
{
	fftInit(CFFT_4096);

}




void eqFilterFrame(int16_t *inputF32, uint16_t cnt)
{

	uint16_t x;
	for (x = 0; x < cnt; x++)
	{
		inputAux[x] = (float32_t)(inputF32[x]);

	}

	arm_status status;
	status = ARM_MATH_SUCCESS;
	float *input_copy = calloc(NFFT_MAGT, sizeof(float));
	//FFT del input
	fft(inputAux, outputAux, true);
	/*
	int i = 0;
	for(i=0; i<EQ_NUM_OF_FILTERS; i++){

		//FFT del filtro
		memcpy(eqFirCoeffs32[i], filtFFTAuxIN, cnt);
		fft(filtFFTAuxIN, filtFFTAuxOUT, true);



	}
*/
}

void eqSetFrameSize(uint32_t eqFrameSize_)
{
	eqFrameSize = eqFrameSize_;
}

void eqSetFilterGains(float32_t gains[EQ_NUM_OF_FILTERS])
{
	for (uint32_t i = 0; i < EQ_NUM_OF_FILTERS; i++)
	{
		eqGains32[i] = gains[i];
	}
}

void eqSetFilterGain(float32_t gain, uint8_t filterNum)
{
	eqGains32[filterNum] = gain;
}
