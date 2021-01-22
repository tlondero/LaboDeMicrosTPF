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
#include "fsl_dmamux.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PDB_COUNT					0x39U		//0x21U

#define PDB_BASEADDR         		PDB0
#define PDB_MODULUS_VALUE      		PDB_COUNT
#define PDB_DELAY_VALUE        		0U
#define PDB_DAC_CHANNEL        		kPDB_DACTriggerChannel0
#define PDB_DAC_INTERVAL_VALUE 		PDB_COUNT
#define DAC_BASEADDR           		DAC0
#define DMAMUX_BASEADDR        		DMAMUX
#define DMA_CHANNEL            		1U
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

static void PDB_Configuration(uint32_t pdb_mod_val,
		pdb_divider_multiplication_factor_t pdb_mult_fact,
		pdb_prescaler_divider_t pdb_prescaler);

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

uint16_t (*backUp);

uint16_t nullData[DAC_USED_BUFFER_SIZE] = { 0U };

bool loopBuffer = true;

bool noMoreClear = false;

bool backUpOn = false;

bool onePeriodDone = false;

uint32_t sizeOf = DAC_USED_BUFFER_SIZE;

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void DAC_Wrapper_Init(void) {

	DAC_Wrapper_Clear_Data_Array();

	//Initialize DMAMUX
	DMAMUX_Configuration();
	//Initialize EDMA
	EDMA_Configuration();
	//Initialize the HW trigger source
	PDB_Configuration(PDB_COUNT, kPDB_DividerMultiplicationFactor1,
			kPDB_PrescalerDivider1);
	//Initialize DAC
	DAC_Configuration();
}

void DAC_Wrapper_PDB_Config(uint32_t mod_val,
		pdb_divider_multiplication_factor_t mult_fact,
		pdb_prescaler_divider_t prescaler) {
	PDB_Configuration(mod_val, mult_fact, prescaler);
}

void DAC_Wrapper_Set_Data_Array(void *newDataArray, uint32_t newSizeOf) {
	g_dacDataArray = (uint16_t*) newDataArray;
	g_index = 0U;
	noMoreClear = false;
	onePeriodDone = false;
	backUp = (uint16_t*) newDataArray;
	if (newSizeOf < DAC_USED_BUFFER_SIZE) {
		sizeOf = newSizeOf;
		backUpOn = true;
	} else {
		backUpOn = false;
	}
}

void DAC_Wrapper_Clear_Data_Array(void) {
	g_dacDataArray = (uint16_t*) nullData;
	backUpOn = false;
	sizeOf = DAC_USED_BUFFER_SIZE;
}

void DAC_Wrapper_Start_Trigger(void) {
	PDB_DoSoftwareTrigger(PDB_BASEADDR);
}

void DAC_Wrapper_Loop(bool status) {
	loopBuffer = status;
}

void MP3_Set_Sample_Rate(uint16_t sr) {
	uint32_t mod_val;
	pdb_divider_multiplication_factor_t mult_fact;
	pdb_prescaler_divider_t prescaler;

	switch (sr) {
	case 8000:
		mod_val = 1875;
		mult_fact = kPDB_DividerMultiplicationFactor1;
		prescaler = kPDB_PrescalerDivider4;
		break;
	case 11025:
		mod_val = 2721;
		mult_fact = kPDB_DividerMultiplicationFactor1;
		prescaler = kPDB_PrescalerDivider2;
		break;
	case 12000:
		mod_val = 125;
		mult_fact = kPDB_DividerMultiplicationFactor40;
		prescaler = kPDB_PrescalerDivider1;
		break;
	case 16000:
		mod_val = 375;
		mult_fact = kPDB_DividerMultiplicationFactor10;
		prescaler = kPDB_PrescalerDivider1;
		break;
	case 22050:
		mod_val = 2721;
		mult_fact = kPDB_DividerMultiplicationFactor1;
		prescaler = kPDB_PrescalerDivider1;
		break;
	case 24000:
		mod_val = 125;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider1;
		break;
	case 32000:
		mod_val = 1875;
		mult_fact = kPDB_DividerMultiplicationFactor1;
		prescaler = kPDB_PrescalerDivider1;
		break;
	case 44100:
		mod_val = 17;
		mult_fact = kPDB_DividerMultiplicationFactor40;
		prescaler = kPDB_PrescalerDivider2;
		break;
	case 48000:
		mod_val = 125;
		mult_fact = kPDB_DividerMultiplicationFactor10;
		prescaler = kPDB_PrescalerDivider1;
		break;
	default:
		mod_val = 17;
		mult_fact = kPDB_DividerMultiplicationFactor40;
		prescaler = kPDB_PrescalerDivider2;
		break;
	}

	DAC_Wrapper_PDB_Config(mod_val, mult_fact, prescaler);
}

bool DAC_Wrapper_Is_Transfer_Done(void) {
	return onePeriodDone;
}

void DAC_Wrapper_Clear_Transfer_Done(void) {
	onePeriodDone = false;
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
static void PDB_Configuration(uint32_t pdb_mod_val,
		pdb_divider_multiplication_factor_t pdb_mult_fact,
		pdb_prescaler_divider_t pdb_prescaler) {
	pdb_config_t pdbConfigStruct;
	pdb_dac_trigger_config_t pdbDacTriggerConfigStruct;

	PDB_GetDefaultConfig(&pdbConfigStruct);

	pdbConfigStruct.dividerMultiplicationFactor = pdb_mult_fact;
	pdbConfigStruct.enableContinuousMode = true;
	pdbConfigStruct.prescalerDivider = pdb_prescaler;

	PDB_Init(PDB_BASEADDR, &pdbConfigStruct);
	PDB_EnableInterrupts(PDB_BASEADDR, kPDB_DelayInterruptEnable);
	PDB_SetModulusValue(PDB_BASEADDR, pdb_mod_val);
	PDB_SetCounterDelayValue(PDB_BASEADDR, PDB_DELAY_VALUE);

	/* Set DAC trigger. */
	pdbDacTriggerConfigStruct.enableExternalTriggerInput = false;
	pdbDacTriggerConfigStruct.enableIntervalTrigger = true;
	PDB_SetDACTriggerConfig(PDB_BASEADDR, PDB_DAC_CHANNEL,
			&pdbDacTriggerConfigStruct);
	PDB_SetDACTriggerIntervalValue(PDB_BASEADDR, PDB_DAC_CHANNEL, pdb_mod_val);

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

	bool endNow = false;

//	if (backUpOn && (g_index >= sizeOf)) { //el tamaño del aray es menor al maximo y ya lo mostré all
//		if (!loopBuffer) {
//			endNow = true;	//sin loop quiero que se termine acá
//		} else {
//			g_dacDataArray = (uint16_t*) nullData;//con loop quiero que se rellene con ceros
//		}
//	}
//
//	if (endNow || (g_index == DAC_USED_BUFFER_SIZE)) {	//transmiti all
//		g_index = 0U;
//		if (backUpOn) {
//			g_dacDataArray = (uint16_t*) backUp;//si hay loop agarro el back up
//		}
//		if (!loopBuffer && !noMoreClear) {	//si no hay loop y no limpie antes
//			DAC_Wrapper_Clear_Data_Array();
//			noMoreClear = true;
//			onePeriodDone = true;
//		}
//
//	}

	if (g_index >= sizeOf) {		//all data send
		g_index = 0U;
		if (!loopBuffer && !noMoreClear) {	//no hay loop y no limpie el buffer
			DAC_Wrapper_Clear_Data_Array();
			noMoreClear = true;
			onePeriodDone = true;
		} else if (loopBuffer && backUpOn) {	//hay loop y cargo el back up
			g_dacDataArray = (uint16_t*) backUp;
		}
		//si hay loop sigo en el mismo buffer (no toco nada)
	}

	EDMA_PrepareTransfer(&g_transferConfig, (void*) (g_dacDataArray + g_index),
			sizeof(uint16_t), (void*) DAC_DATA_REG_ADDR, sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t), kEDMA_MemoryToMemory);
	EDMA_SetTransferConfig(DMA_BASEADDR, DMA_CHANNEL, &g_transferConfig, NULL);
	/* Enable transfer. */
	EDMA_StartTransfer(&g_EDMA_Handle);
}
