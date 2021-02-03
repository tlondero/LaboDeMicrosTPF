
/***************************************************************************/ /**
  @file     FileName.c
  @brief
  @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "button.h"
#include "fsl_port.h"
#include "debug_ifdefs.h"
#ifdef BUTTON_DEBOUNCE
#include "fsl_sdmmc_osa.h"
#endif
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define APP_WAKEUP_BUTTON_GPIO        BOARD_SW2_GPIO
#define APP_WAKEUP_BUTTON_PORT        BOARD_SW2_PORT
#define APP_WAKEUP_BUTTON_GPIO_PIN    BOARD_SW2_GPIO_PIN
#define APP_WAKEUP_BUTTON_IRQ         BOARD_SW2_IRQ
#define APP_WAKEUP_BUTTON_IRQ_HANDLER BOARD_SW2_IRQ_HANDLER
#define APP_WAKEUP_BUTTON_NAME        BOARD_SW2_NAME
#define APP_WAKEUP_BUTTON_IRQ_TYPE    kPORT_InterruptFallingEdge
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
void BUTTON_Init(void){
	/* Init wakeup interruption */
	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
	NVIC_EnableIRQ(APP_WAKEUP_BUTTON_IRQ);	//NVIC del wakeup (SW2)
	PORT_SetPinInterruptConfig(APP_WAKEUP_BUTTON_PORT,
	APP_WAKEUP_BUTTON_GPIO_PIN, APP_WAKEUP_BUTTON_IRQ_TYPE); //PCR del wakeup (SW2)

	/* Botonera */
	NVIC_EnableIRQ(PORTD_IRQn);	//NVIC botonera PORTD
	NVIC_EnableIRQ(BOARD_SW3_IRQ);	//NVIC SW3 PORTA
	PORT_SetPinInterruptConfig(PORTD, 0U, kPORT_InterruptRisingEdge); //PCR PORTD0
	PORT_SetPinInterruptConfig(PORTD, 1U, kPORT_InterruptRisingEdge); //PCR PORTD1
	PORT_SetPinInterruptConfig(PORTD, 2U, kPORT_InterruptRisingEdge); //PCR PORTD2
	PORT_SetPinInterruptConfig(PORTD, 3U, kPORT_InterruptRisingEdge); //PCR PORTD3
	PORT_SetPinInterruptConfig(BOARD_SW3_PORT, BOARD_SW3_GPIO_PIN,
			kPORT_InterruptFallingEdge); //PCR SW3
}

bool BUTTON_GetBackButton(void){
	if(back_button){
		back_button = false;
		return true;
	}
	else{
		return false;
	}
}
bool BUTTON_GetPrevButton(void){
	if(prev_button){
		prev_button = false;
		return true;
	}
	else{
		return false;
	}
}
bool BUTTON_GetNextButton(void){
	if(next_button){
		next_button = false;
		return true;
	}
	else{
		return false;
	}
}
bool BUTTON_GetPausePlayButton(void){
	if(pause_play_button){
		pause_play_button = false;
		return true;
	}
	else{
		return false;
	}
}
bool BUTTON_GetOnOffButton(void){
	if(off_on_button){
		off_on_button = false;
		return true;
	}
	else{
		return false;
	}
}
bool BUTTON_GetEnterButton(void){
	if(enter_button){
		enter_button = false;
		return true;
	}
	else{
		return false;
	}
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
void BUTTON_NEXT_CALLBACK(void){
	next_button = true;
}
void BUTTON_PREV_CALLBACK(void){
	prev_button = true;
}
void BUTTON_BACK_CALLBACK(void){
	back_button = true;
}
void BUTTON_ENTER_CALLBACK(void){
	enter_button = true;
}
void BUTTON_PLAY_CALLBACK(void){
	pause_play_button = true;
}
void BUTTON_ON_OFF_CALLBACK(void){
	if(kinetisWakeupArmed){
		kinetisWakeupArmed = false;
	}
	else{
		off_on_button = true;
		kinetisWakeupArmed = true;
	}
}

void PORTC_IRQHandler(void){

    if (((1U << 6U) & PORT_GetPinsInterruptFlags(PORTC)))
    {
        PORT_ClearPinsInterruptFlags(PORTC, (1U << 6U));
        __DSB();
        BUTTON_ON_OFF_CALLBACK();
        /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
        exception return operation might vector to incorrect interrupt */
    }
}

void PORTA_IRQHandler(void)   //SW2
{
	 if (((1U << 4U) & PORT_GetPinsInterruptFlags(PORTA))){
	    PORT_ClearPinsInterruptFlags(PORTA, (1U << 4U));
	    BUTTON_PLAY_CALLBACK();
	 }
}

void PORTD_IRQHandler(void)	//BOTONERA
{
	if (((1U << 0U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD0
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 0U));
			SDMMC_OSADelay(20U);
#ifdef BUTTON_DEBOUNCE
			if(GPIO_PinRead(GPIOD, 0U))
#endif
			{
				BUTTON_NEXT_CALLBACK();
			}
	}
	else if (((1U << 1U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD1
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 1U));
			SDMMC_OSADelay(20U);
#ifdef BUTTON_DEBOUNCE
			if(GPIO_PinRead(GPIOD, 1U))
#endif
			{
				BUTTON_PREV_CALLBACK();
			}
	}
	else if (((1U << 2U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD2
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 2U));
			SDMMC_OSADelay(20U);
#ifdef BUTTON_DEBOUNCE
			if(GPIO_PinRead(GPIOD, 2U))
#endif
			{
				BUTTON_ENTER_CALLBACK();
			}
	}
	else if (((1U << 3U) & PORT_GetPinsInterruptFlags(PORTD))){ //PTD3
			PORT_ClearPinsInterruptFlags(PORTD, (1U << 3U));
			SDMMC_OSADelay(20U);
#ifdef BUTTON_DEBOUNCE
			if(GPIO_PinRead(GPIOD, 3U))
#endif
			{
			BUTTON_BACK_CALLBACK();
			}
	}
}
