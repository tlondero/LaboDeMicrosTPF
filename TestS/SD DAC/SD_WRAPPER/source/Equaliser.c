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
#define NFFT 4096


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
static float32_t *outputAux;
static float32_t *inputAux;

void eqInit(uint32_t frameSize)
{
	inputAux = (float32_t *) calloc(NFFT, sizeof(float32_t));
	outputAux = (float32_t *) calloc(NFFT, sizeof(float32_t));

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
	float *input_copy = calloc(NFFT, sizeof(float));
	arm_copy_f32((const float32_t *)inputF32, (const float32_t *)input_copy, NFFT);

	//arm_cfft_radix4_instance_f32 cfft_instance_ptr;
	//status = arm_cfft_radix4_init_f32(cfft_instance_ptr, NFFT/2, 0, 1);
	//arm_cfft_radix4_f32(cfft_instance_ptr, input_copy);


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
