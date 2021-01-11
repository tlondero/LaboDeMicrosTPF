/*
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    DAC_TEST.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_sdhc.h"
#include "fsl_clock.h"


#define TRANSFER_SIZE	0x02*0x01
#define BUFFER_CANT		50
#define NBYTES			0x02
#define SIZE_OF_BUFFER	BUFFER_CANT*NBYTES

uint16_t bufferPtr[BUFFER_CANT];
uint16_t bufferXD[BUFFER_CANT] = {0};

void App_Run();
void Init_Hardware();

void DAC_Init();
void Dmamux_Init();
void DMA_Init();
void PIT_Init();

int main(void) {

	/* Init board hardware. */
	Init_Hardware();

	DAC_Init();
	Dmamux_Init();
	DMA_Init();
	PIT_Init();

	int i = 0;
	for (i = 0; i < 16; i++) {
		bufferPtr[i] = (char) i;
	}

	while (1) {
		App_Run();
	}
	return 0;
}

void App_Run() {

	PRINTF("App Run\n");

}

void DMA0_IRQHandler() {
	// Clear the interrupt flag
	DMA0->CINT |= 0;
}

void Init_Hardware() {
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
}

void DAC_Init() {
	//Dac init
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;

	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK; // | DAC_C0_DACTRGSEL_MASK;
	DAC0->C1 = DAC_C1_DMAEN_MASK;
	//DAC0->C1 &= ~DAC_C1_DACBFEN_MASK;
}

void Dmamux_Init() {

	//DMAMUX Init
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(58); // | DMAMUX_CHCFG_TRIG_MASK; //DAC0 CH0
	//45
}

void DMA_Init() {
	//EDMA Init

	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

	NVIC_ClearPendingIRQ(DMA0_IRQn);		// Clear all the pending events
	NVIC_EnableIRQ(DMA0_IRQn);				// Enable the DMA interrupts

	/// ============= INIT TCD0 ===================//
	/* Set memory address for source and destination. */
	DMA0->TCD[0].SADDR = (uint32_t) (bufferPtr);				//List of Duties

	//DMA0->TCD[0].DADDR = (uint32_t)(destinationBuffer);
	DMA0->TCD[0].DADDR = (uint32_t) (bufferXD);//(&(DAC0->DAT[0].DATL));

	/* Set an offset for source and destination address. */
	DMA0->TCD[0].SOFF = 0x02; // Source address offset of 2 bytes per transaction.
	DMA0->TCD[0].DOFF = 0x02; // Destination address offset is 0. (Siempre al mismo lugar)

	/* Set source and destination data transfer size to 16 bits (CnV is 2 bytes wide). */
	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);

	/*Number of bytes to be transfered in each service request of the channel.*/
	DMA0->TCD[0].NBYTES_MLNO = (TRANSFER_SIZE);

	/* Current major iteration count. */
	DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(BUFFER_CANT);
	DMA0->TCD[0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(BUFFER_CANT);

	/* Autosize SLAST for Wrap Around. This value is added to SADD at the end of Major Loop */
	DMA0->TCD[0].SLAST = -SIZE_OF_BUFFER;

	/* DLASTSGA DLAST Scatter and Gatter */
	DMA0->TCD[0].DLAST_SGA = 0x00;

	/* Setup control and status register. */

	DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;	//Enable Major Interrupt.

	/* Enable request signal for channel 0. */
	DMA0->ERQ = DMA_ERQ_ERQ0_MASK;
}

void PIT_Init() {
	//PIT Init
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	PIT->CHANNEL[0].LDVAL = 1133;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}
