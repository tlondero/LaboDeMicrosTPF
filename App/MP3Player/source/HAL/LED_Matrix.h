/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "fsl_common.h"
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
 * @brief  -- Led matrix init..
 * @param  --
 * @return --
*/
void LEDMATRIX_Init(void);

/**
 * @brief  -- Set an individual led with a certain RGB value given row and column
 * @param  -- row: row of the led
 * @param  -- col: column of the led
 * @param  -- r: red value
 * @param  -- g: green value
 * @param  -- b: blue value
 * @return --
*/
void LEDMATRIX_SetLed(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief  -- Turn on led matrix
 * @param  --
 * @return --
*/
void LEDMATRIX_Enable(void);

/**
 * @brief  -- Turn off led matrix
 * @param  --
 * @return --
*/
void LEDMATRIX_Disable(void);

void LEDMATRIX_Pause(void);

void LEDMATRIX_Resume(void);

void LEDMATRIX_SetCb(void* cb_);

void LEDMATRIX_EnableAnimation(void);

void LEDMATRIX_DisableAnimation(void);
void LEDMATRIX_CleanDisplay(void);
#endif /* LED_MATRIX_H_ */
