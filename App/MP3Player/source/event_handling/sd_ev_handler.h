/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef EVENT_HANDLING_SD_EV_HANDLER_H_
#define EVENT_HANDLING_SD_EV_HANDLER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "stdbool.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
	bool sd_just_in;
	bool sd_just_out;
	bool opened_nondir;
} sd_event_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  get_event_SD
 * @param  sd_event: event structure for the sd
 * @return --
*/
void SD_EVHANDLER_GetEvents(sd_event_t * sd_event);


#endif /* EVENT_HANDLING_SD_EV_HANDLER_H_ */
