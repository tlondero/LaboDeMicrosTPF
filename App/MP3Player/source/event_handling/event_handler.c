/***************************************************************************//**
 @file     event_handler.c
 @brief
 @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "event_handler.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

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
void EVHANDLER_GetEvents(event_t *evs) {
	BUTTONS_EVHANDLER_GetEvents(&(evs->btn_evs));
	SD_EVHANDLER_GetEvents(&(evs->sd_evs));
	FSEXP_EVHANDLER_GetEvents(&(evs->fsexp_evs));
}

void EVHANDLER_InitHandlers(void){
	FSEXP_EVHANDLER_Init();
	SD_EVHANDLER_Init();
	BUTTONS_EVHANDLER_Init();
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

