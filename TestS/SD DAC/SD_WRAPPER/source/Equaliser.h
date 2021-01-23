/***************************************************************************//**
 @file     Equaliser.h
 @brief
 @author   MAGT
 ******************************************************************************/
#ifndef EQUALISER_H_
#define EQUALISER_H_
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "arm_math.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FIR_FILTERS			10
#define	FIR_COEFF			1001

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	kEq_32Hz,
	kEq_64Hz,
	kEq_125Hz,
	kEq_250Hz,
	kEq_500Hz,
	kEq_1000Hz,
	kEq_2000Hz,
	kEq_4000Hz,
	kEq_8000Hz,
	kEq_16000Hz,
} equaliser_filters_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void Equaliser_Init(void);

void Equaliser_Set_All_Gains(float32_t gains_[FIR_FILTERS]);

void Equaliser_Set_Gain(float32_t gain, equaliser_filters_t filter);

void Equaliser_Frame(float32_t * input, float32_t * output);

void Equaliser_Set_Frame_Size(uint32_t size);

#endif /* EQUALISER_H_ */
