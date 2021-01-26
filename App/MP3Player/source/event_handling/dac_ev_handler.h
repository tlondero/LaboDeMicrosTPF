/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   MAGT
 ******************************************************************************/
#ifndef EVENT_HANDLING_DAC_EV_HANDLER_H_
#define EVENT_HANDLING_DAC_EV_HANDLER_H_

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
	bool dac_start_playing;
	bool dac_end_playing;
} dac_event_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  get_event_DAC
 * @param  sd_event: event structure for the dac
 * @return --
*/
void DAC_EVHANDLER_GetEvents(dac_event_t * _dac_event);

/**
 * @brief  initializates the event handler
 * @param  --
 * @return --
*/
void DAC_EVHANDLER_Init(void);
#endif /* EVENT_HANDLING_DAC_EV_HANDLER_H_ */
