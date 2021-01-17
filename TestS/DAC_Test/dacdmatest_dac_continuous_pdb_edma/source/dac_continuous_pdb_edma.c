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

int main(void) {

	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	DAC_Wrapper_Init();

	// 4095U == 2 V

	uint16_t arrayXD[DAC_USED_BUFFER_SIZE] = { 0U, 401U, 799U, 1188U, 1567U,
			1930U, 2275U, 2598U, 2895U, 3165U, 3405U, 3611U, 3783U, 3918U,
			4016U, 4075U, 4095U, 4075U, 4016U, 3918U, 3783U, 3611U, 3405U,
			3165U, 2895U, 2598U, 2275U, 1930U, 1567U, 1188U, 799U, 401U };

	uint16_t sen1k[DAC_USED_BUFFER_SIZE] = { 2048U, 2461U, 2856U, 3219U, 3533U,
			3787U, 3970U, 4074U, 4095U, 4032U, 3888U, 3668U, 3383U, 3043U,
			2662U, 2256U, 1841U, 1435U, 1054U, 714U, 428U, 209U, 65U, 2U, 23U,
			127U, 310U, 563U, 878U, 1240U, 1636U, 2048U };

	uint16_t exp1k[DAC_USED_BUFFER_SIZE] = { 0U, 1U, 3U, 6U, 9U, 13U, 18U, 23U,
			31U, 39U, 50U, 63U, 79U, 99U, 123U, 152U, 188U, 232U, 286U, 352U,
			433U, 532U, 654U, 803U, 985U, 1208U, 1481U, 1816U, 2225U, 2727U,
			3342U, 4095U };

	DAC_Wrapper_Loop(true);		//Por defecto está en true pero lo explicito

	DAC_Wrapper_Set_Data_Array(&exp1k);

	DAC_Wrapper_Start_Trigger();

	while (1) {

	}
}
