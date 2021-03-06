/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef EVENT_HANDLING_BUTTON_EV_HANDLER_H_
#define EVENT_HANDLING_BUTTON_EV_HANDLER_H_

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
	bool back_button;
	bool next_button;
	bool prev_button;
	bool enter_button;
	bool off_on_button;
	bool pause_play_button;
} button_event_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  get_event_buttons
 * @param  button_event: event structure for the buttons
 * @return --
*/
void BUTTONS_EVHANDLER_GetEvents(button_event_t * button_event);

/**
 * @brief  initializates the ev handler
 * @param  --
 * @return --
*/
void BUTTONS_EVHANDLER_Init(void);
#endif /* EVENT_HANDLING_BUTTON_EV_HANDLER_H_ */
