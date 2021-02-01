/***************************************************************************//**
  @file     equaliser.h
  @brief    Applies FIR filters to the input singal, you can personlize the gain of these filters, the filter coefficients can be changed in the eqFirCoeffs32 vector.
  @author   MAGT
 ******************************************************************************/

#ifndef MCAL_EQUALISER_EQUALISER_H_
#define MCAL_EQUALISER_EQUALISER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "arm_math.h"
#include "filters_coefs.h"
#include "fft.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialises equaliser. Calls ARM DSP initialisation functions.
 * @param frameSize  Number of data numbers to be filtered when calling eqFilterFrame.
 */
void eqInit(uint32_t frameSize);

/**
 * @brief Compute the equaliser filter on the data given.
 * @param inputF32  Pointer to input data to filter.
 * @param outputF32 Pointer to where the filtered data should be saved.
 */
void eqFilterFrame(int16_t *inputF32, uint16_t cnt);

/**
 * @brief Sets all equaliser filter gains.
 * @param gains  Array with the filter gains for each of the equaliser bands.
 */
void eqSetFilterGains(float32_t gains[EQ_NUM_OF_FILTERS]);

/**
 * @brief Sets equaliser number filterNum to the gain given.
 * @param gain        Filter gain for filter number filterNum.
 * @param filterNum   Number of filter to apply the gain to.
 */
void eqSetFilterGain(float32_t gain, uint8_t filterNum);

void eqSetFrameSize(uint32_t eqFrameSize_);

/*******************************************************************************
 ******************************************************************************/


#endif /* MCAL_EQUALISER_EQUALISER_H_ */
