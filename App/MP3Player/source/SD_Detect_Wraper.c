
/***************************************************************************/ /**
  @file     SD_Detect_Wraper.c
  @brief
  @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsl_sd_disk.h"
#include "fsl_common.h"
#include "debug_ifdefs.h"
#include "fsl_pit.h"
#include "fsl_sysmpu.h"
#include "fsl_sd.h"
#include "SD_Detect_Wraper.h"
#include "MK64F12.h"
#include "sdmmc_config.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/
void interruptRoutine(void); //Interrupt
/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
extern bool nextFrameFlag;

static bool inserted_flag=false;
static bool removed_flag=false;
static bool fs_mounted_flag=false;

static bool old_status = kSD_Removed;

static sd_card_t *card = &g_sd;
static FATFS g_fileSystem; /* File system object */

static FRESULT error;
static pit_config_t pitConfig;
static const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
static cback insertedCback;
static cback extractedCback;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
bool SDWRAPPER_Init(cback inserted_callback, cback extracted_callback){
	SYSMPU_Enable(SYSMPU, false);
		//////////////////////////////////////////////////////////////
		//         				PIT CONFIGURATION
		//////////////////////////////////////////////////////////////
		PIT_GetDefaultConfig(&pitConfig);

		/* Init pit module */
		PIT_Init(PIT, &pitConfig);

		/* Set timer period for channel 0 */
		PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,
				USEC_TO_COUNT(1000000U, CLOCK_GetFreq(kCLOCK_BusClk)));

		/* Enable timer interrupts for channel 0 */
		PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

		/* Enable at the NVIC */
		EnableIRQ(PIT0_IRQn);

		PIT_StartTimer(PIT, kPIT_Chnl_0);
		//////////////////////////////////////////////////////////////
		//						SD CONFIG 						    //
		//////////////////////////////////////////////////////////////
		BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

		/* SD host init function */
		if (SD_HostInit(&g_sd) != kStatus_Success) {
			return !kStatus_Fail;
		}
		/* power off card */
		SD_SetCardPower(&g_sd, false);
		if(inserted_callback != NULL){
			insertedCback=inserted_callback;
		}
		if(extracted_callback != NULL){
			extractedCback=extracted_callback;
		}


		return !kStatus_Success;
}

void SDWRAPPER_SetInterruptEnable(bool enable){
	if(enable){
		PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	}
	else{
		PIT_DisableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	}
}

void SDWRAPPER_SetCardPower(bool enable){
	if(SDWRAPPER_getSDInserted()){
		SD_SetCardPower(&g_sd, enable);
	}
}

void SDWRAPPER_ClearInsertedFlag(void){
	if(SDWRAPPER_getSDInserted()){
	inserted_flag = false;
	}
}

void SDWRAPPER_SetInsertedFlag(void){
	if(SDWRAPPER_getSDInserted()){
	inserted_flag = true;
	}
}

bool SDWRAPPER_getSDInserted(void){return card->usrParam.cd->cardDetected();}

bool SDWRAPPER_GetMounted(void){return fs_mounted_flag;}

bool SDWRAPPER_getJustIn(void){
	if(((card->usrParam.cd->cardDetected() == true)
			&& (inserted_flag)) ==  true)
	{inserted_flag=false;
	return true;}
	else return false;
}

bool SDWRAPPER_getJustOut(void){
	if(((card->usrParam.cd->cardDetected() == false)
			&& (removed_flag)) ==  true)
	{removed_flag=false;
	return true;}
	else return false;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
void interruptRoutine(void){

//		my code goes here.

	if (card->usrParam.cd->type == kSD_DetectCardByGpioCD) {
		if (card->usrParam.cd->cardDetected == NULL) //Si no existe la funcion
		{
			error = FR_DISK_ERR;
		}
		if ((card->usrParam.cd->cardDetected() == true)
				&& (old_status != kSD_Inserted)) {

			SDMMC_OSADelay(card->usrParam.cd->cdDebounce_ms);
			if (card->usrParam.cd->cardDetected() == true) {
				SD_SetCardPower(&g_sd, true);
				if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
					error = FR_DISK_ERR;
							}

				#if (FF_FS_RPATH >= 2U)
							error = f_chdrive((char const*) &driverNumberBuffer[0U]);
							if (error) {
								error = FR_DISK_ERR;
							}
				#endif
				old_status = kSD_Inserted;
				inserted_flag=true;
				removed_flag=false;
				fs_mounted_flag = true;
				if(insertedCback){
					insertedCback();
				}
			}
		}

		else if ((card->usrParam.cd->cardDetected() == false)
				&& (old_status != kSD_Removed)) {
			old_status = kSD_Removed;
			removed_flag=true;
			SD_SetCardPower(&g_sd, false);
			inserted_flag=false;
			fs_mounted_flag = false;
			if(extractedCback){
				extractedCback();
			}
		}

	}
//

}

void PIT0_IRQHandler(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);

	interruptRoutine();
	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();

}
