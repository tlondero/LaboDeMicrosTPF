/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "fsl_sd.h"
#include "fsl_debug_console.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "board.h"
#include "sdmmc_config.h"
#include "fsl_sysmpu.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "FS_explorer.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* buffer size (in byte) for read/write operations */
#define BUFFER_SIZE (100U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief wait card insert function.
 */
static status_t sdcardWaitCardInsert(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FATFS g_fileSystem; /* File system object */


/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
/*! @brief Data written to the card */
SDK_ALIGN(uint8_t g_bufferWrite[BUFFER_SIZE],
		BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_bufferRead[BUFFER_SIZE],
		BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void) {
	FRESULT error;

	const TCHAR driverNumberBuffer[3U] = { SDDISK + '0', ':', '/' };

	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	SYSMPU_Enable(SYSMPU, false);

	PRINTF(
			"\r\nFATFS example to demonstrate how to use FATFS with SD card.\r\n");

	PRINTF("\r\nPlease insert a card into board.\r\n");

	if (sdcardWaitCardInsert() != kStatus_Success) {
		return -1;
	}

	if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
		PRINTF("Mount volume failed.\r\n");
		return -1;
	}

#if (FF_FS_RPATH >= 2U)
	error = f_chdrive((char const*) &driverNumberBuffer[0U]);
	if (error) {
		PRINTF("Change drive failed.\r\n");
		return -1;
	}
#endif

	printf("Exploring all files forward\r\n");
	char *chptr = exploreFS("/");

	printf("\r\n %s \r\n", chptr);

	int i = 1;
	while (1) {
		chptr = getNext();
		if (!chptr) {
			break;
		}

		printf("\r\n %s \r\n", chptr);
		printf("\r\n %d \r\n", i++);
	}
	printf("Exploring all files backwards\r\n");
	i--;
	while (1) {
		chptr = getPrev();
		if (!chptr) {
			break;
		}

		printf("\r\n %s \r\n", chptr);
		printf("\r\n %d \r\n", i--);
	}
	printf("\r\n Trying to add a open directory\r\n");
	for (int i = 0; i < 4; i++) {
		if (i == 3) {
			chptr = openSelected(chptr);
			break;
		}
		chptr = getNext();
	}
	printf("\r\n %s \r\n", chptr);

	i = 1;
	while (1) {
		chptr = getNext();
		if (!chptr) {
			break;
		}

		printf("\r\n %s \r\n", chptr);
		printf("\r\n %d \r\n", i++);
	}
	printf("\r\n Going back in the directory\r\n");
	chptr = goBackDir();
	printf("\r\n %s \r\n", chptr);
	PRINTF("\r\nThe example will not read/write file again.\r\n");

	while (true) {
	}
}

static status_t sdcardWaitCardInsert(void) {

	BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

	/* SD host init function */
	if (SD_HostInit(&g_sd) != kStatus_Success) {
		PRINTF("\r\nSD host init fail\r\n");
		return kStatus_Fail;
	}
	/* power off card */
	SD_SetCardPower(&g_sd, false);

	/* wait card insert */
	if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success) {
		PRINTF("\r\nCard inserted.\r\n");
		/* power on the card */
		SD_SetCardPower(&g_sd, true);
	} else {
		PRINTF("\r\nCard detect fail.\r\n");
		return kStatus_Fail;
	}

	return kStatus_Success;
}

