
/***************************************************************************/ /**
  @file     sd_ev_handler.c
  @brief	handler for the ev_handler
  @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "sd_ev_handler.h"
//#include "../SD_Detect_Wraper.h"

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
static sd_event_t sd_event;
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void SD_EVHANDLER_GetEvents(sd_event_t * _sd_event){
	//sd_event->sd_just_out = SDWRAPPER_getJustOut();
	//sd_event->sd_just_in =  SDWRAPPER_getJustIn();
}

void SD_EVHANDLER_Init(void){

}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
