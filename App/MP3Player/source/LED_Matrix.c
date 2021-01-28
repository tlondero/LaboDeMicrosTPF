
/***************************************************************************/ /**
  @file     FileName.c
  @brief
  @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "LED_Matrix.h"
#include "fsl_ftm.h"
#include "fsl_edma.h"
#include "fsl_pit.h"
#include "fsl_dmamux.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/*FTM*/
#define LEDMATRIX_FTM_CNV_ON 39 //39 ticks -> 0.8us
#define LEDMATRIX_FTM_CNV_OFF 20 //20 ticks -> 0.4us
#define LEDMATRIX_FTM_CNV_ZERO 0
#define LEDMATRIX_FTM_MOD 62//62+1ticks ->1.26us
#define LEDMATRIX_FTM_BASE FTM0
#define LEDMATRIX_FTM_CHANNEL 0

/*MEM*/
#define LEDMATRIX_CANT_LEDS 64
#define LEDMATRIX_CANT_LEDS_ZERO 0
#define LEDMATRIX_MAT_SIZE ((LEDMATRIX_CANT_LEDS+LEDMATRIX_CANT_LEDS_ZERO)*8*3*2)+(1*2) //(64 LEDS+10LEDS en zero para reset) * 8BITS * 3 COLORES * 2bytes (CNV son 16 bits)
#define LEDMATRIX_ROW_SIZE 8

/*PIT*/
#define LEDMATRIX_NUMBEROFCHANNELS 1
#define LEDMATRIX_PIT_PERIOD_MS 2

/*eDMA*/
#define LEDMATRIX_DMA_BASE DMA0
#define LEDMATRIX_DMA_CHANNEL 2U

/*DMAMUX*/
#define LEDMATRIX_DMAMUX_BASE DMAMUX
#define LEDMATRIX_DMAMUX_SOURCE kDmaRequestMux0FTM0Channel0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {RED, GREEN, BLUE} led_color;
typedef struct{
	uint16_t G[8]; //Un array de 8 elementos de 16 bits.
	uint16_t R[8];
	uint16_t B[8];
}GRB_t;
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/
static void Edma_Callback(edma_handle_t *handle, void *userData,
bool transferDone, uint32_t tcds);
static void set_color_brightness(uint16_t *ptr, uint8_t brightness);
/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
static GRB_t led_matrix [LEDMATRIX_CANT_LEDS+LEDMATRIX_CANT_LEDS_ZERO];
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void LEDMATRIX_Init(void){

	uint8_t i;
	for(i = 0; i < LEDMATRIX_CANT_LEDS; i++){

		set_color_brightness(led_matrix[i].R, 0);
		set_color_brightness(led_matrix[i].G, 0);
		set_color_brightness(led_matrix[i].B, 0);

	}

	/* PIT config */
	pit_config_t pit_config;
	PIT_GetDefaultConfig(&pit_config);
	PIT_Init(PIT, &pit_config);
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, USEC_TO_COUNT(LEDMATRIX_PIT_PERIOD_MS*1000U, CLOCK_GetFreq(kCLOCK_BusClk)));

	/* FTM config */
	ftm_config_t ftm_config;
	FTM_GetDefaultConfig(&ftm_config); //Si no funca, probar cambiar pwmSyncMode.
	FTM_Init(LEDMATRIX_FTM_BASE, &ftm_config);
	ftm_chnl_pwm_config_param_t pwm_params;
	pwm_params.chnlNumber = 0;
	pwm_params.dutyValue = LEDMATRIX_FTM_CNV_OFF;
	pwm_params.firstEdgeValue = 0;
	pwm_params.level = kFTM_HighTrue;
	FTM_SetupPwmMode(LEDMATRIX_FTM_BASE, &pwm_params, LEDMATRIX_NUMBEROFCHANNELS, kFTM_EdgeAlignedPwm);
	FTM_SetTimerPeriod(LEDMATRIX_FTM_BASE, LEDMATRIX_FTM_MOD);
	FTM_SetSoftwareTrigger(LEDMATRIX_FTM_BASE, false);
	FTM_EnableInterrupts(LEDMATRIX_FTM_BASE, kFTM_Chnl0InterruptEnable);
	//FTM_EnableDmaTransfer(LEDMATRIX_FTM_BASE, kFTM_Chnl_0, true);

	/* DMAMUX config */
	DMAMUX_Init(LEDMATRIX_DMAMUX_BASE);
	DMAMUX_SetSource(LEDMATRIX_DMAMUX_BASE, LEDMATRIX_DMA_CHANNEL, LEDMATRIX_DMAMUX_SOURCE);
	DMAMUX_EnableChannel(LEDMATRIX_DMAMUX_BASE, LEDMATRIX_DMA_CHANNEL);

	/* eDMA config */
	edma_config_t userConfig;
	EDMA_GetDefaultConfig(&userConfig);
	EDMA_Init(LEDMATRIX_DMA_BASE, &userConfig);
	edma_handle_t EDMA_Handle;
	EDMA_CreateHandle(&EDMA_Handle, LEDMATRIX_DMA_BASE, LEDMATRIX_DMA_CHANNEL);
	EDMA_SetCallback(&EDMA_Handle, Edma_Callback, NULL);
	edma_transfer_config_t transferConfig;
	EDMA_PrepareTransfer(&transferConfig,
						(void*)(led_matrix),						/*srcAddr*/
						sizeof(uint16_t), 							/*srcWidth*/
						(void*) (&(LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV)), 		/*destAddr*/
						sizeof(uint16_t), 							/*destWidth*/
						sizeof(uint16_t),							/*bytesEachRequest (NBYTES??)*/
						LEDMATRIX_MAT_SIZE, 						/*transferBytes (SLAST??)*/
						kEDMA_MemoryToPeripheral);					/*Source se mueve de a uint16, destiny no se mueve*/
	EDMA_SubmitTransfer(&EDMA_Handle, &transferConfig);
	/* Enable interrupt when transfer is done. */
	EDMA_EnableChannelInterrupts(LEDMATRIX_DMA_BASE, LEDMATRIX_DMA_CHANNEL,
			kEDMA_MajorInterruptEnable);

}

void LEDMATRIX_SetLed(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b){

	set_color_brightness(led_matrix[LEDMATRIX_ROW_SIZE*row+col].G, g);
	set_color_brightness(led_matrix[LEDMATRIX_ROW_SIZE*row+col].R, r);
	set_color_brightness(led_matrix[LEDMATRIX_ROW_SIZE*row+col].B, b);
}

void LEDMATRIX_Enable(void){
	FTM_EnableDmaTransfer(LEDMATRIX_FTM_BASE, kFTM_Chnl_0, true);
}

void LEDMATRIX_Disable(void){
	FTM_EnableDmaTransfer(LEDMATRIX_FTM_BASE, kFTM_Chnl_0, false);
	LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_ON;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

static void set_color_brightness(uint16_t *ptr, uint8_t brightness){
	uint8_t i;
	for (i = 0; i<8; i++){
		ptr[i] = (brightness & (1<<i)) ? LEDMATRIX_FTM_CNV_ON : LEDMATRIX_FTM_CNV_OFF;
	}

}

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
static void Edma_Callback(edma_handle_t *handle, void *userData,
bool transferDone, uint32_t tcds) {
	/* Clear Edma interrupt flag. */
	EDMA_ClearChannelStatusFlags(LEDMATRIX_DMA_BASE, LEDMATRIX_DMA_CHANNEL,
			kEDMA_InterruptFlag);
	FTM_StopTimer(LEDMATRIX_FTM_BASE);
	PIT_StartTimer(PIT, kPIT_Chnl_1);
}

void PIT1_IRQHandler(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
	PIT_StopTimer(PIT, kPIT_Chnl_1);
	if(led_matrix[0].G[0] == LEDMATRIX_FTM_CNV_ON){
		LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_ON;
	}
	else{
		LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_OFF;
	}
	FTM_StartTimer(LEDMATRIX_FTM_BASE, kFTM_SystemClock);

	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();

}

