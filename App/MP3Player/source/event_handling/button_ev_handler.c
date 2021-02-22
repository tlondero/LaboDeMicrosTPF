/***************************************************************************//**
 @file     button_ev_handler.c
 @brief
 @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "button_ev_handler.h"
#include "board.h"
#include "fsl_gpio.h"
#include "stdbool.h"
#include "../HAL/button.h"
#include "fsl_common.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//#define DEBOUNCE_SDKDELAY
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void BUTTONS_EVHANDLER_GetEvents(button_event_t *button_event) {
	button_event->back_button = BUTTON_GetBackButton();
	button_event->enter_button = BUTTON_GetEnterButton();
	button_event->next_button = BUTTON_GetNextButton();
	button_event->off_on_button = BUTTON_GetOnOffButton();
	button_event->pause_play_button = BUTTON_GetPausePlayButton();
	button_event->prev_button = BUTTON_GetPrevButton();
}

void BUTTONS_EVHANDLER_Init(void){

}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

