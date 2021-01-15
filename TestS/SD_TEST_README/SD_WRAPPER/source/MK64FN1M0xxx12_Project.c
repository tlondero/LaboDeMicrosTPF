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
#include "fsl_common.h"
#include "fsl_pit.h"
#include "SD_Detect_Wraper.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */




static FIL g_fileObject;   /* File object */


/*
 * @brief   Application entry point.
 */
int main(void) {


    FRESULT error;
    DIR directory; /* Directory object */
    FILINFO fileInformation;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
	SDWraperInit();

/////////////////////////////////////////////////////////////
	while (1) {
		if (getJustIn()) {

			PRINTF("\r\nCreate directory......\r\n");
			error = f_mkdir(_T("/dir_2"));
			if (error) {
				if (error == FR_EXIST) {
					PRINTF("Directory exists.\r\n");
				} else {
					PRINTF("Make directory failed.\r\n");
					return -1;
				}
			}

		}
	}

	return 0;
}


