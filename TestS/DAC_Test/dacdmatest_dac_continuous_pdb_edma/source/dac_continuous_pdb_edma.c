/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"

#include "DAC_Wrapper.h"

#include "fsl_common.h"
#include "clock_config.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    DAC_Wrapper_Init();
	
	uint16_t arrayXD[DAC_USED_BUFFER_SIZE] = { 0U, 401U, 799U, 1188U, 1567U,
		1930U, 2275U, 2598U, 2895U, 3165U, 3405U, 3611U, 3783U, 3918U, 4016U,
		4075U, 4095U, 4075U, 4016U, 3918U, 3783U, 3611U, 3405U, 3165U, 2895U,
		2598U, 2275U, 1930U, 1567U, 1188U, 799U, 401U };
		
	DAC_Wrapper_Set_Data_Array(&arrayXD);

	DAC_Wrapper_Start_Trigger();

    while (1)
    {
    }
}
