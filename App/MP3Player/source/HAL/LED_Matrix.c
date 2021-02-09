
/***************************************************************************/ /**
  @file     FileName.c
  @brief
  @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include "LED_Matrix.h"
#include "fsl_ftm.h"
#include "fsl_edma.h"
#include "fsl_pit.h"
#include "fsl_dmamux.h"
#include "fsl_port.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/*FTM*/
#define LEDMATRIX_FTM_CNV_ON 48 // -> 0.8us
#define LEDMATRIX_FTM_CNV_OFF 24 // -> 0.4us
#define LEDMATRIX_FTM_CNV_ZERO 0
#define LEDMATRIX_FTM_MOD 76// -> 1.26us
#define LEDMATRIX_FTM_BASE FTM0
#define LEDMATRIX_FTM_CHANNEL 0

/*MEM*/
#define LEDMATRIX_CANT_LEDS 64
#define LEDMATRIX_CANT_LEDS_ZERO 0
#define LEDMATRIX_MAT_SIZE ((LEDMATRIX_CANT_LEDS+LEDMATRIX_CANT_LEDS_ZERO)*8*3*2)//+(1*2) //(64 LEDS+10LEDS en zero para reset) * 8BITS * 3 COLORES * 2bytes (CNV son 16 bits)
#define LEDMATRIX_ROW_SIZE 8

/*PIT*/
#define LEDMATRIX_NUMBEROFCHANNELS 1
#define LEDMATRIX_PIT_PERIOD_MS 2

/*eDMA*/
#define LEDMATRIX_DMA_BASE DMA0
#define LEDMATRIX_DMA_CHANNEL 0U

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
static void set_color_brightness(uint16_t *ptr, uint8_t brightness);
/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
static GRB_t led_matrix [LEDMATRIX_CANT_LEDS+LEDMATRIX_CANT_LEDS_ZERO];
static uint16_t old_cnv;
static bool pit_alt_pause;
static bool pit_alt_pause_2;
static bool block;
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
	NVIC_EnableIRQ(PIT1_IRQn); //PIT
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, MSEC_TO_COUNT(5U, CLOCK_GetFreq(kCLOCK_BusClk)));
	/* Enable timer interrupts for channel 1 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);

	/* eDMA config */
	/* Enable the clock for the eDMA and the DMAMUX. */
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	/* Enable the eDMA channel 0 and set the FTM CH0 as the DMA request source. */
	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(20);   // FTM0 CH0

	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA0_IRQn);

	/* Set memory address for source and destination. */
	DMA0->TCD[0].SADDR = (uint32_t)(led_matrix);

	//DMA_TCD0_DADDR = (uint32_t)(destinationBuffer);
	DMA0->TCD[0].DADDR = (uint32_t)(&(LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV));

	/* Set an offset for source and destination address. */
	DMA0->TCD[0].SOFF =0x02; // Source address offset of 2 bytes per transaction.
	DMA0->TCD[0].DOFF =0x00; // Destination address offset is 0. (Siempre al mismo lugar)

	/* Set source and destination data transfer size to 16 bits (CnV is 2 bytes wide). */
	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);

	/*Number of bytes to be transfered in each service request of the channel.*/
	DMA0->TCD[0].NBYTES_MLNO = 0x02;

	/* Current major iteration count. */
	DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(LEDMATRIX_MAT_SIZE/(0x02));
	DMA0->TCD[0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(LEDMATRIX_MAT_SIZE/(0x02));

	/* Autosize SLAST for Wrap Around. This value is added to SADD at the end of Major Loop */
	DMA0->TCD[0].SLAST = -LEDMATRIX_MAT_SIZE;

	/* DLASTSGA DLAST Scatter and Gatter */
	 DMA0->TCD[0].DLAST_SGA = 0x00;

	/* Setup control and status register. */
	DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;	//Enable Major Interrupt.

	/* Enable request signal for channel 0. */
	DMA0->ERQ = DMA_ERQ_ERQ0_MASK;

	/* FTM config */
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	NVIC_ClearPendingIRQ(FTM0_IRQn);
	NVIC_EnableIRQ(FTM0_IRQn);
	LEDMATRIX_FTM_BASE->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | (1<<0U);
	LEDMATRIX_FTM_BASE->SC = (LEDMATRIX_FTM_BASE->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(kFTM_Prescale_Divide_1);
	LEDMATRIX_FTM_BASE->CNTIN = 0U;						//CNTIN
	LEDMATRIX_FTM_BASE->CNT = 0U;						//CNT
	LEDMATRIX_FTM_BASE->MOD = LEDMATRIX_FTM_MOD;		//MOD
	LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC = (LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC
		& ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK))
		| (FTM_CnSC_MSB((2 >> 1) & 0X01)
		| FTM_CnSC_MSA((2 >> 0) & 0X01));
	LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC =
		(LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC
		& ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK))
		| (FTM_CnSC_ELSB((0x02 >> 1) & 0X01)
		| FTM_CnSC_ELSA((0x02 >> 0) & 0X01));
	LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = FTM_CnV_VAL(LEDMATRIX_FTM_CNV_OFF);
	LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC = (LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC
		& ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(1);
	LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC = (LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC & ~(FTM_CnSC_DMA_MASK)) |
										  (FTM_CnSC_DMA(1));
}

void LEDMATRIX_SetLed(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b){

	if(!block){
	set_color_brightness(led_matrix[LEDMATRIX_ROW_SIZE*row+col].G, g);
	set_color_brightness(led_matrix[LEDMATRIX_ROW_SIZE*row+col].R, r);
	set_color_brightness(led_matrix[LEDMATRIX_ROW_SIZE*row+col].B, b);
	}
}

void LEDMATRIX_Resume(void){
	block = false;
	pit_alt_pause = false;
	pit_alt_pause_2 = false;
	if(led_matrix[0].G[0] == LEDMATRIX_FTM_CNV_ON){
		LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_ON;
	}
	else{
		LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_OFF;
	}
	LEDMATRIX_Enable();
	//LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC = (LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnSC
	//	& ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(1);
}

void LEDMATRIX_Pause(void){
	uint8_t i;
	for(i = 0; i < LEDMATRIX_CANT_LEDS; i++){
		set_color_brightness(led_matrix[i].R, 0);
		set_color_brightness(led_matrix[i].G, 0);
		set_color_brightness(led_matrix[i].B, 0);
	}
	block = true;
	pit_alt_pause = true;
}

void LEDMATRIX_Enable(void){
	LEDMATRIX_FTM_BASE->SC |= FTM_SC_CLKS(0x01);
}

void LEDMATRIX_Disable(void){
	//LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_ZERO;
	LEDMATRIX_FTM_BASE->SC &= ~FTM_SC_CLKS(0x01);
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

void PIT1_IRQHandler(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
	PIT_StopTimer(PIT, kPIT_Chnl_1);
	if(!pit_alt_pause_2){
		if(pit_alt_pause){
			pit_alt_pause_2 = true;
		}
		if(led_matrix[0].G[0] == LEDMATRIX_FTM_CNV_ON){
			LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_ON;
		}
		else{
			LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_OFF;
		}
		LEDMATRIX_Enable();
	}
	else{
		LEDMATRIX_FTM_BASE->CONTROLS[LEDMATRIX_FTM_CHANNEL].CnV = LEDMATRIX_FTM_CNV_ZERO;
		LEDMATRIX_Disable();
	}

	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();

}

void FTM0_IRQHandler(void) {
	uint32_t status;
	status = FTM0->STATUS; //Capturo flags de interrupcion de todos los canales

	FTM0->STATUS = 0;	//Limpio todos los flags
}

void DMA0_DriverIRQHandler(void)
{
	/* Clear Edma interrupt flag. */
	DMA0->CINT |= 0;
	LEDMATRIX_Disable();
	PIT_StartTimer(PIT, kPIT_Chnl_1);
    SDK_ISR_EXIT_BARRIER;
}

