/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef BUTTON_H_
#define BUTTON_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "fsl_common.h"
#include "board.h"
#include "fsl_smc.h"
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
 * @brief  -- Initializates buttons
 * @param  --
 * @return --
*/
void BUTTON_Init(void);

bool BUTTON_GetBackButton(void);
bool BUTTON_GetPrevButton(void);
bool BUTTON_GetNextButton(void);
bool BUTTON_GetPausePlayButton(void);
bool BUTTON_GetOnOffButton(void);
bool BUTTON_GetEnterButton(void);

#endif /* BUTTON_H_ */
