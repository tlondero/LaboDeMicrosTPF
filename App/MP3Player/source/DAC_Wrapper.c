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

#define PDB_COUNT					17

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

edma_handle_t g_EDMA_Handle;                             	//Edma handler

edma_transfer_config_t g_transferConfig;                 //Edma transfer config

volatile uint32_t g_index = 0U; 			//Index of the g_dacDataArray array

uint16_t (*g_dacDataArray);								//Array que va al dac

uint16_t (*nextBuffer);	//Cuando cambio a un segundo buffer y necesito avisar de antemano

bool nextBufferLoad = false;	//Muestra si hay un siguiente buffer

uint16_t nullData[DAC_USED_BUFFER_SIZE] = { 0U };//Array vacio para enviar al dac

bool loopBufferActive = true;//Determina si al finalizar un periodo se repite el buffer o no se manda nada mas

bool nullArrayOn = true;

bool onePeriodDone = false;		//Determina si se envio un peridodo del buffer

bool sleepMode = false;

uint32_t sizeOf = DAC_USED_BUFFER_SIZE;			//Cantidad de datos en el buffer

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
	//44100Hz mp3 default config
	PDB_Configuration(PDB_COUNT, kPDB_DividerMultiplicationFactor40,
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

	if (!nextBufferLoad) {
		g_dacDataArray = (uint16_t*) newDataArray;
		g_index = 0U;
	}
	nullArrayOn = false;
	onePeriodDone = false;
	sizeOf = newSizeOf;
}

void DAC_Wrapper_Set_Next_Buffer(void *forcedBackUp) {
	if (forcedBackUp == NULL) {
		nextBuffer = (uint16_t*) nullData;
		nextBufferLoad = false;
	} else {
		nextBuffer = (uint16_t*) forcedBackUp;
		nextBufferLoad = true;
	}
}

void DAC_Wrapper_Clear_Data_Array(void) {
	g_dacDataArray = (uint16_t*) nullData;
	nullArrayOn = true;
	sizeOf = DAC_USED_BUFFER_SIZE;
}

void DAC_Wrapper_Clear_Next_Buffer(void) {
	nextBuffer = (uint16_t*) nullData;
}

void DAC_Wrapper_Start_Trigger(void) {
	PDB_DoSoftwareTrigger(PDB_BASEADDR);
}

void DAC_Wrapper_Loop(bool status) {
	loopBufferActive = status;
}

void DAC_Wrapper_Sleep(void) {
	sleepMode = true;
}

void DAC_Wrapper_Wake_Up(void) {
	sleepMode = false;
	PDB_EnableInterrupts(PDB_BASEADDR, kPDB_DelayInterruptEnable);
	DAC_EnableBufferInterrupts(DAC_BASEADDR,
			kDAC_BufferReadPointerTopInterruptEnable);

	EDMA_EnableChannelInterrupts(DMA_BASEADDR, DMA_CHANNEL,
			kEDMA_MajorInterruptEnable);
	EDMA_StartTransfer(&g_EDMA_Handle);
	DAC_Wrapper_Clear_Data_Array();
}

bool MP3_Set_Sample_Rate(uint16_t sr, uint8_t ch) {
	bool ret = true;
	uint32_t mod_val;
	pdb_divider_multiplication_factor_t mult_fact;
	pdb_prescaler_divider_t prescaler;

	switch (ch) {		//Mono
	case (1):
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
			mod_val = 16;
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
			ret = false;
			break;
		}
		break;
	case (2):		//Stereo
		switch (sr) {
		case 8000:
			mod_val = 1875;
			mult_fact = kPDB_DividerMultiplicationFactor1;
			prescaler = kPDB_PrescalerDivider2;
			break;
		case 11025:
			mod_val = 2721;
			mult_fact = kPDB_DividerMultiplicationFactor1;
			prescaler = kPDB_PrescalerDivider1;
			break;
		case 12000:
			mod_val = 125;
			mult_fact = kPDB_DividerMultiplicationFactor10;
			prescaler = kPDB_PrescalerDivider2;
			break;
		case 16000:
			mod_val = 1875;
			mult_fact = kPDB_DividerMultiplicationFactor1;
			prescaler = kPDB_PrescalerDivider1;
			break;
		case 22050:
			mod_val = 85;
			mult_fact = kPDB_DividerMultiplicationFactor1;
			prescaler = kPDB_PrescalerDivider16;
			break;
		case 24000:
			mod_val = 125;
			mult_fact = kPDB_DividerMultiplicationFactor10;
			prescaler = kPDB_PrescalerDivider1;
			break;
		case 32000:
			mod_val = 937;
			mult_fact = kPDB_DividerMultiplicationFactor1;
			prescaler = kPDB_PrescalerDivider1;
			break;
		case 44100:
			mod_val = 16;
			mult_fact = kPDB_DividerMultiplicationFactor40;
			prescaler = kPDB_PrescalerDivider1;
			break;
		case 48000:
			mod_val = 625;
			mult_fact = kPDB_DividerMultiplicationFactor1;
			prescaler = kPDB_PrescalerDivider1;
			break;
		default:
			mod_val = 17;
			mult_fact = kPDB_DividerMultiplicationFactor40;
			prescaler = kPDB_PrescalerDivider1;
			ret = false;
			break;
		}
		break;
	default:
		ret = false;
		break;
	}

	DAC_Wrapper_PDB_Config(mod_val, mult_fact, prescaler);
	return ret;
}

void MP3_Adapt_Signal(int16_t *src, uint16_t *dst, uint16_t cnt,
		uint8_t volumen) {

	uint16_t j = 0;
	for (j = 0; j < cnt; j++) {
		if (volumen) {
			uint16_t aux = (uint16_t) (((src[j]) / (VOLUME_STEPS + 1))
					* ((volumen)));	//maximo es 30

			dst[j] = ((aux + 32768) / 16);
		} else {
			dst[j] = 0;
		}
	}
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

	if (g_index >= sizeOf) {		//all data send
		g_index = 0U;
		if (!loopBufferActive && !nullArrayOn) {//Loop apagado y no limpie nunca (no hay array nulo en buffer)
			if (nextBufferLoad) {
				g_dacDataArray = nextBuffer;
			} else {
				DAC_Wrapper_Clear_Data_Array();
				if (sleepMode) {
					PDB_DisableInterrupts(PDB_BASEADDR,
							kPDB_DelayInterruptEnable);

					EDMA_StopTransfer(&g_EDMA_Handle);
					EDMA_DisableChannelInterrupts(DMA_BASEADDR, DMA_CHANNEL,
							kEDMA_MajorInterruptEnable);

					DAC_DisableBufferInterrupts(DAC_BASEADDR,
							kDAC_BufferReadPointerTopInterruptEnable);
				}
			}
			onePeriodDone = true;
		}
		//si hay loop sigo en el mismo buffer (no toco nada)
	}

	EDMA_PrepareTransfer(&g_transferConfig, (void*) (g_dacDataArray + g_index),
			sizeof(uint16_t), (void*) DAC_DATA_REG_ADDR, sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t),
			DAC_DATL_COUNT * sizeof(uint16_t), kEDMA_MemoryToMemory);
	EDMA_SetTransferConfig(DMA_BASEADDR, DMA_CHANNEL, &g_transferConfig,
	NULL);
	/* Enable transfer. */
	EDMA_StartTransfer(&g_EDMA_Handle);
}
