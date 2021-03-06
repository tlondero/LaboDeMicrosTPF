/***************************************************************************//**
  @file fft.c
  @brief performs the FFT algorithm with the DSP library funcionts
  @author G. Lambertucci, T. Londero M. Rodriguez, A. Melachupa
 ******************************************************************************/


#ifndef FFT_H_
#define FFT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "arm_math.h"
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum 
{
  CFFT_16,
  CFFT_32,
  CFFT_64,
  CFFT_128,
  CFFT_256,
  CFFT_512,
  CFFT_1024,
  CFFT_2048,
  CFFT_4096
} cfft_size_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the FFT module
 * @param Size of the fft to compute.
 */
void fftInit(cfft_size_t size);

/**
 * @brief Computes the FFT.
 * @param inputF32      Buffer with input data.
 * @param outputF32     Buffer to store the output data.
 * @param doBitReverse  Determines whether to bit reverse output.
 */
void fft(float32_t * inputF32, float32_t * outputF32, bool doBitReverse);

/**
 * @brief Compute the inverse FFT.
 * @param inputF32      Buffer with input data.
 * @param outputF32     Buffer to store the output data.
 * @param doBitReverse  Determines whether to bit reverse output.
 */
void ifft(float32_t * inputF32, float32_t * outputF32, bool doBitReverse);

/**
 * @brief Calculates the Module of the vector.
 * @param inputF32      Buffer with input data.
 * @param outputF32     Buffer to store the output data.
 */
void fftGetMag(float32_t * inputF32, float32_t * outputF32);



/**
 * @brief changes the 1024 fft to 8 bines.
 * @param inputF32      Buffer with input data.
 * @param outputF32     buffer with the 8 bines.
 */

void fftMakeBines8(float32_t *src,float32_t * dst);
/*******************************************************************************
 ******************************************************************************/


#endif
