/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef EVENT_HANDLING_EVENT_HANDLER_H_
#define EVENT_HANDLING_EVENT_HANDLER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "sd_ev_handler.h"
#include "button_ev_handler.h"
#include "fsexplorer_ev_handler.h"
#include "encoder_ev_handler.h"
#include "general.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
typedef struct{
	sd_event_t sd_evs;
	button_event_t btn_evs;
	fsexplorer_event_t fsexp_evs;
	encoder_event_t encoder_evs;
}event_t;
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
 * @brief  event_handler_get_event
 * @param evs pointer to the Evs to be filled by the event handler
 * @return --
*/
void EVHANDLER_GetEvents(event_t * evs);

/**
 * @brief  initializates all the event handlers
 * @param --
 * @return --
*/
void EVHANDLER_InitHandlers(void);

#endif /* EVENT_HANDLING_EVENT_HANDLER_H_ */
