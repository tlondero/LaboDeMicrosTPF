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
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
typedef struct{
	sd_event_t sd_evs;
	button_event_t btn_evs;
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
void event_handler_get_event(event_t * evs);
#endif /* EVENT_HANDLING_EVENT_HANDLER_H_ */
