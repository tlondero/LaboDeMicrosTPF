/***************************************************************************//**
 @file     FileName.c
 @brief
 @author   Guido Lambertucci
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "button_ev_handler.h"
#include "board.h"
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

static bool kinetisWakeupArmed = true;

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
	back_button = false;
	next_button = false;
	prev_button = false;
	enter_button = false;
	off_on_button = false;
	pause_play_button = false;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
void APP_WAKEUP_BUTTON_IRQ_HANDLER(void){ //Esta es la rutina de interrupción del botoncino

    if (kinetisWakeupArmed && ((1U << APP_WAKEUP_BUTTON_GPIO_PIN) & PORT_GetPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT)))
    {
        PORT_ClearPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT, (1U << APP_WAKEUP_BUTTON_GPIO_PIN));
        /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
        exception return operation might vector to incorrect interrupt */
        __DSB();
        kinetisWakeupArmed = false;
    }
    else if(!kinetisWakeupArmed && ((1U << APP_WAKEUP_BUTTON_GPIO_PIN) & PORT_GetPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT))){
    	PORT_ClearPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT, (1U << APP_WAKEUP_BUTTON_GPIO_PIN));
    	off_on_button = true;
    }
}
