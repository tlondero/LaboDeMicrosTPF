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
 * @file    MK64FN1M0xxx12_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_sysmpu.h"
#include "fsl_debug_console.h"
#include "sdmmc_config.h"
#include "fsl_sd_disk.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
static status_t sdcardWaitCardInsert(void);

/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    SYSMPU_Enable(SYSMPU, false);
    SYSMPU_Enable(SYSMPU, false);
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    sdcardWaitCardInsert();
    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}

static status_t sdcardWaitCardInsert(void)
{
    //////////////////////////////////////////////////////////////////////////////////////
	//                					INIT SD											//
    //////////////////////////////////////////////////////////////////////////////////////
	BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        PRINTF("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }
    /* power off card */
    SD_SetCardPower(&g_sd, false);

    //////////////////////////////////////////////////////////////////////////////////////
	//                				PONIENDING Y SACANDING								//
    //////////////////////////////////////////////////////////////////////////////////////

    /* wait card insert */
    bool is_inserted=false;
    do {
		if (!is_inserted && (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success))
		{
			PRINTF("\r\nCard inserted.\r\n");
			is_inserted=true;
			/* power on the card */
			SD_SetCardPower(&g_sd, true);
		}
		else if(is_inserted && (SD_PollingCardInsert(&g_sd, kSD_Removed) == kStatus_Success) )
		{
			is_inserted=false;
			PRINTF("\r\nCard extracted.\r\n");
			SD_SetCardPower(&g_sd, false);
		}
	} while (1);

    return kStatus_Success;
}
