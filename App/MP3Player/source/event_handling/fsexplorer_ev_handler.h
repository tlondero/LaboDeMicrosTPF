/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef EVENT_HANDLING_FSEXPLORER_EV_HANDLER_H_
#define EVENT_HANDLING_FSEXPLORER_EV_HANDLER_H_

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
	bool play_music;
} fsexplorer_event_t;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief  Get events of the file explorer
 * @param  fsexplorer_event -- the events of the file explorer
 * @return --
*/
void SD_EVHANDLER_GetEvents(fsexplorer_event_t * fsexplorer_event);
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

#endif /* EVENT_HANDLING_FSEXPLORER_EV_HANDLER_H_ */
