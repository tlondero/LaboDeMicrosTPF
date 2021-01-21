/***************************************************************************//**
 @file     DAC_Wraper.h
 @brief
 @author   MAGT
 ******************************************************************************/
#ifndef DAC_WRAPER_H_
#define DAC_WRAPER_H_
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_common.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "fsl_pdb.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DAC_USED_BUFFER_SIZE 		1024U

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void DAC_Wrapper_Init(void);

void DAC_Wrapper_Set_Data_Array(void *newDataArray, uint32_t newSizeOf);

void DAC_Wrapper_Clear_Data_Array(void);

void DAC_Wrapper_Start_Trigger(void);		//Start trigger signal to DAC

void DAC_Wrapper_Loop(bool status);

void DAC_Wrapper_PDB_Config(uint32_t mod_val,
		pdb_divider_multiplication_factor_t mult_fact,
		pdb_prescaler_divider_t prescaler);

void MP3_Set_Sample_Rate(uint16_t sr);

bool DAC_Wrapper_Is_Transfer_Done(void);

void DAC_Wrapper_Clear_Transfer_Done(void);

#endif /* DAC_WRAPER_H_ */
