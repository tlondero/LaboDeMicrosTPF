/***************************************************************************//**
 @file     FileName.c
 @brief
 @author   Guido Lambertucci
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "fsl_pit.h"
#include "fsl_common.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_DELAYS 10
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
static bool *piter_flag[MAX_DELAYS];
static uint32_t usecs_array[MAX_DELAYS];
static uint8_t flag_index;
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
uint8_t delaysinitDelayBlockInterrupt(uint32_t usecs, bool *var2change) {
	static uint8_t i = 0;

	if (i < MAX_DELAYS) {
		usecs_array[i] = usecs;
		piter_flag[i] = var2change;
		return i++;
	} else
		return 0;

}

void delaysStart(uint8_t id) {
	flag_index = id;
	bool someone_busy=false;
	for(int i=0 ;i<MAX_DELAYS;i++){
		someone_busy |= *(piter_flag[i]);
	}
	if(!someone_busy){
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_2,
			USEC_TO_COUNT(usecs_array[flag_index],
					CLOCK_GetFreq(kCLOCK_BusClk)));
	PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
	EnableIRQ(PIT2_IRQn);

	PIT_StartTimer(PIT, kPIT_Chnl_2);
	}
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

void PIT2_IRQHandler(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);

	*(piter_flag[flag_index]) = false;
	PIT_StopTimer(PIT, kPIT_Chnl_2);
	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();

}
