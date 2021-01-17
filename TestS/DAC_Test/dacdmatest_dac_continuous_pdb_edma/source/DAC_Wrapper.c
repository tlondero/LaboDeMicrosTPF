/***************************************************************************//**
 @file     DAC_Wrapper.c
 @brief
 @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DAC_Wrapper.h"

#include "fsl_dac.h"
#include "fsl_edma.h"
#include "fsl_pdb.h"
#include "fsl_dmamux.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PDB_BASEADDR         		PDB0
#define PDB_MODULUS_VALUE      		0x21U
#define PDB_DELAY_VALUE        		0U
#define PDB_DAC_CHANNEL        		kPDB_DACTriggerChannel0
#define PDB_DAC_INTERVAL_VALUE 		0x21U
#define DAC_BASEADDR           		DAC0
#define DMAMUX_BASEADDR        		DMAMUX
#define DMA_CHANNEL            		0U
#define DMA_DAC_SOURCE         		45U
#define DMA_BASEADDR           		DMA0
#define DAC_DATA_REG_ADDR           0x400cc000U
#define DMA_IRQ_ID             		DMA0_IRQn

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/

static void EDMA_Configuration(void);

static void DMAMUX_Configuration(void);

static void PDB_Configuration(void);

static void DAC_Configuration(void);

static void Edma_Callback(edma_handle_t *handle, void *userData,
		bool transferDone, uint32_t tcds);

/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/

edma_handle_t g_EDMA_Handle;                             //Edma handler

edma_transfer_config_t g_transferConfig;                 //Edma transfer config

volatile uint32_t g_index = 0U; //Index of the g_dacDataArray array

uint16_t (*g_dacDataArray);

uint16_t nullData[DAC_USED_BUFFER_SIZE] = { 0U };

bool loopBuffer;

uint16_t max_count;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void DAC_Wrapper_Init(void) {

	loopBuffer = true;
	max_count = DAC_USED_BUFFER_SIZE;
	
	DAC_Wrapper_Clear_Data_Array();

	//Initialize DMAMUX
	DMAMUX_Configuration();
	//Initialize EDMA
	EDMA_Configuration();
	//Initialize the HW trigger source
	PDB_Configuration();
	//Initialize DAC
	DAC_Configuration();
}

void DAC_Wrapper_Set_Data_Array(void * newDataArray){
	g_dacDataArray = (uint16_t *) newDataArray;
}

void DAC_Wrapper_Clear_Data_Array(void){
	g_dacDataArray = (uint16_t *) &nullData;
}

void DAC_Wrapper_Start_Trigger(void) {
	PDB_DoSoftwareTrigger(PDB_BASEADDR);
}

void DAC_Wrapper_Loop(bool status){
	loopBuffer = status;
}

bool DAC_Wrapper_Set_Max_Count(uint16_t max){
	bool ret = false;
	if(max < DAC_USED_BUFFER_SIZE){
		max_count = max;
		ret = true;
	}
	
	return ret;
}

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

static void EDMA_Configuration(void) {
	edma_config_t userConfig;

	EDMA_GetDefaultConfig(&userConfig);
	EDMA_Init(DMA_BASEADDR, &userConfig);
	EDMA_CreateHandle(&g_EDMA_Handle, DMA_BASEADDR, DMA_CHANNEL);
	EDMA_SetCallback(&g_EDMA_Handle, Edma_Callback, NULL);
	EDMA_PrepareTransfer(&g_transferConfig, (void*) (g_dacDataArray + g_index),
			sizeof(uint16_t), (void*) DAC_DATA_REG_ADDR, sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t), kEDMA_MemoryToMemory);
	EDMA_SubmitTransfer(&g_EDMA_Handle, &g_transferConfig);
	/* Enable interrupt when transfer is done. */
	EDMA_EnableChannelInterrupts(DMA_BASEADDR, DMA_CHANNEL,
			kEDMA_MajorInterruptEnable);
#if defined(FSL_FEATURE_EDMA_ASYNCHRO_REQUEST_CHANNEL_COUNT) && FSL_FEATURE_EDMA_ASYNCHRO_REQUEST_CHANNEL_COUNT
    /* Enable async DMA request. */
    EDMA_EnableAsyncRequest(DMA_BASEADDR, DMA_CHANNEL, true);
#endif /* FSL_FEATURE_EDMA_ASYNCHRO_REQUEST_CHANNEL_COUNT */
	/* Enable transfer. */
	EDMA_StartTransfer(&g_EDMA_Handle);
}

static void DMAMUX_Configuration(void) {
	/* Configure DMAMUX */
	DMAMUX_Init(DMAMUX_BASEADDR);
	DMAMUX_SetSource(DMAMUX_BASEADDR, DMA_CHANNEL, DMA_DAC_SOURCE); /* Map ADC source to channel 0 */
	DMAMUX_EnableChannel(DMAMUX_BASEADDR, DMA_CHANNEL);
}

/* Enable the trigger source of PDB. */
static void PDB_Configuration(void) {
	pdb_config_t pdbConfigStruct;
	pdb_dac_trigger_config_t pdbDacTriggerConfigStruct;

	PDB_GetDefaultConfig(&pdbConfigStruct);
	pdbConfigStruct.dividerMultiplicationFactor =
			kPDB_DividerMultiplicationFactor40;
	pdbConfigStruct.enableContinuousMode = true;
	PDB_Init(PDB_BASEADDR, &pdbConfigStruct);
	PDB_EnableInterrupts(PDB_BASEADDR, kPDB_DelayInterruptEnable);
	PDB_SetModulusValue(PDB_BASEADDR, PDB_MODULUS_VALUE);
	PDB_SetCounterDelayValue(PDB_BASEADDR, PDB_DELAY_VALUE);

	/* Set DAC trigger. */
	pdbDacTriggerConfigStruct.enableExternalTriggerInput = false;
	pdbDacTriggerConfigStruct.enableIntervalTrigger = true;
	PDB_SetDACTriggerConfig(PDB_BASEADDR, PDB_DAC_CHANNEL,
			&pdbDacTriggerConfigStruct);
	PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL,
			PDB_DAC_INTERVAL_VALUE);

	/* Load PDB values. */
	PDB_DoLoadValues(PDB_BASEADDR);
}

static void DAC_Configuration(void) {
	dac_config_t dacConfigStruct;
	dac_buffer_config_t dacBufferConfigStruct;

	DAC_GetDefaultConfig(&dacConfigStruct);
	DAC_Init(DAC_BASEADDR, &dacConfigStruct);
	DAC_Enable(DAC_BASEADDR, true); /* Enable output. */

	/* Configure the DAC buffer. */
	DAC_EnableBuffer(DAC_BASEADDR, true);
	DAC_GetDefaultBufferConfig(&dacBufferConfigStruct);
	dacBufferConfigStruct.triggerMode = kDAC_BufferTriggerByHardwareMode;
	DAC_SetBufferConfig(DAC_BASEADDR, &dacBufferConfigStruct);
	DAC_SetBufferReadPointer(DAC_BASEADDR, 0U); /* Make sure the read pointer to the start. */

	/* Enable DMA. */
	DAC_EnableBufferInterrupts(DAC_BASEADDR,
			kDAC_BufferReadPointerTopInterruptEnable);
	DAC_EnableBufferDMA(DAC_BASEADDR, true);
}

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

static void Edma_Callback(edma_handle_t *handle, void *userData,
		bool transferDone, uint32_t tcds) {
	/* Clear Edma interrupt flag. */
	EDMA_ClearChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL,
			kEDMA_InterruptFlag);
	/* Setup transfer */
	g_index += DAC_DATL_COUNT;
	//if ((g_index == max_count) || (g_index == DAC_USED_BUFFER_SIZE)) {
	if (g_index == max_count) {
		g_index = 0U;
		if(!loopBuffer){
			DAC_Wrapper_Clear_Data_Array();
			//EDMA_StopTransfer(handle);		//Debería haber un start?
			return;
		}
	}
	EDMA_PrepareTransfer(&g_transferConfig, (void*) (g_dacDataArray + g_index),
			sizeof(uint16_t), (void*) DAC_DATA_REG_ADDR, sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t), kEDMA_MemoryToMemory);
	EDMA_SetTransferConfig(DMA_BASEADDR, DMA_CHANNEL, &g_transferConfig, NULL);
	/* Enable transfer. */
	EDMA_StartTransfer(&g_EDMA_Handle);
}
