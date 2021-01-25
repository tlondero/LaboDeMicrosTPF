/***************************************************************************//**
 @file     FileName.c
 @brief
 @author   Guido Lambertucci
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "stdbool.h"
#include "button_ev_handler.h"
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

static bool back_button;
static bool next_button;
static bool prev_button;
static bool enter_button;
static bool off_on_button;
static bool pause_play_button;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void get_event_buttons(button_event_t *button_event) {
	button_event->back_button = back_button;
	button_event->enter_button = enter_button;
	button_event->next_button = next_button;
	button_event->off_on_button = off_on_button;
	button_event->pause_play_button = pause_play_button;
	button_event->prev_button = prev_button;
	back_button = 0;
	next_button = 0;
	prev_button = 0;
	enter_button = 0;
	off_on_button = 0;
	pause_play_button = 0;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

