/***************************************************************************//**
 @file     FileName.c
 @brief
 @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "button_ev_handler.h"
#include "board.h"
#include "fsl_gpio.h"
#include "stdbool.h"
#include "fsl_common.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//#define DEBOUNCE_SDKDELAY
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
void BUTTONS_EVHANDLER_GetEvents(button_event_t *button_event) {
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

void BUTTONS_EVHANDLER_Init(void){

}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
void PORTC_IRQHandler(void){ //Esta es la rutina de interrupción del botoncino

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
    	kinetisWakeupArmed = true;  //TODO: esto puede que no este bien aca
    }
}

void PORTA_IRQHandler(void)   //SW2
{
	 if (((1U << BOARD_SW3_GPIO_PIN) & PORT_GetPinsInterruptFlags(BOARD_SW3_PORT))){
	    PORT_ClearPinsInterruptFlags(BOARD_SW3_PORT, (1U << BOARD_SW3_GPIO_PIN));
	    pause_play_button = true;
	 }

}

void PORTD_IRQHandler(void)	//BOTONERA
{
	if (((1U << 0U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD0
#ifdef DEBOUNCE_SDKDELAY
		SDK_DelayAtLeastUs(25 * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
		if(GPIO_PinRead(GPIOD, 0U)){
#endif
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 0U));
			next_button = true;
#ifdef DEBOUNCE_SDKDELAY
		}
#endif
	}
	else if (((1U << 1U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD1
#ifdef DEBOUNCE_SDKDELAY
		SDK_DelayAtLeastUs(25 * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
		if(GPIO_PinRead(GPIOD, 1U)){
#endif
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 1U));
			enter_button = true;
#ifdef DEBOUNCE_SDKDELAY
		}
#endif
	}
	else if (((1U << 2U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD2
#ifdef DEBOUNCE_SDKDELAY
		SDK_DelayAtLeastUs(25 * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
		if(GPIO_PinRead(GPIOD, 2U)){
#endif
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 2U));
			prev_button = true;
#ifdef DEBOUNCE_SDKDELAY
		}
#endif
	}
	else if (((1U << 3U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD3
#ifdef DEBOUNCE_SDKDELAY
		SDK_DelayAtLeastUs(25 * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
		if(GPIO_PinRead(GPIOD, 3U)){
#endif
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 3U));
			back_button = true;
#ifdef DEBOUNCE_SDKDELAY
		}
#endif
	}
}
