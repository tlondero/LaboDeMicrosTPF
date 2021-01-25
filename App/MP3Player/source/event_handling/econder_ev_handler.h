/*
 * encoder_handler.h
 *
 *  Created on: Jan 24, 2021
 *      Author: Acer
 */

#ifndef EVENT_HANDLING_ECONDER_EV_HANDLER_H_
#define EVENT_HANDLING_ECONDER_EV_HANDLER_H_

typedef struct {
	int huevo;
}encoder_event_t;

encoder_event_t ENC_EVHANDLER_GetEvents();


#endif /* EVENT_HANDLING_ECONDER_EV_HANDLER_H_ */
