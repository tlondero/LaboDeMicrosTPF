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
void event_handler_get_event(event_t *evs) {
	get_event_buttons(&(evs->btn_evs));
	get_event_SD(&(evs->sd_evs));

}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

