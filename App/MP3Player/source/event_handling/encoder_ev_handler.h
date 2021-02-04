/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef EVENT_HANDLING_ENCODER_EV_HANDLER_H_
#define EVENT_HANDLING_ENCODER_EV_HANDLER_H_

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
} encoder_event_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  encoderHandlerGetEvents
 * @param  encoder_event: event structure for the encoder
 * @return --
*/
void encoderHandlerGetEvents(encoder_event_t *encoder_event);

/**
 * @brief  initializates the ev handler
 * @param  --
 * @return --
*/

void encoderHandlerInit(void);

#endif /* EVENT_HANDLING_ENCODER_EV_HANDLER_H_ */
