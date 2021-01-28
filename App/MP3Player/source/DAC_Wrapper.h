/***************************************************************************//**
 @file     DAC_Wraper.h
 @brief
 @author   MAGT
 ******************************************************************************/
#ifndef DAC_WRAPER_H_
#define DAC_WRAPER_H_
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_common.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "fsl_pdb.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DAC_USED_BUFFER_SIZE 		10240U
#define VOLUME_STEPS				30

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	kMP3_8000Hz = 8000,
	kMP3_11025z = 11025,
	kMP3_12000Hz = 12000,
	kMP3_16000Hz = 16000,
	kMP3_22050Hz = 22050,
	kMP3_24000Hz = 24000,
	kMP3_32000Hz = 32000,
	kMP3_44100Hz = 44100,
	kMP3_48000Hz = 48000
} mp3_sample_rate_t;

typedef enum {
	kMP3_Mono = 1,
	kMP3_Stereo = 2
} mp3_audio_mode_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void DAC_Wrapper_Init(void);

void DAC_Wrapper_Set_Data_Array(void *newDataArray, uint32_t newSizeOf);

void DAC_Wrapper_Set_Next_Buffer(void *forcedBackUp);

void DAC_Wrapper_Clear_Data_Array(void);

void DAC_Wrapper_Start_Trigger(void);		//Start trigger signal to DAC

void DAC_Wrapper_Loop(bool status);

void DAC_Wrapper_PDB_Config(uint32_t mod_val,
		pdb_divider_multiplication_factor_t mult_fact,
		pdb_prescaler_divider_t prescaler);

bool MP3_Set_Sample_Rate(uint16_t sr, uint8_t ch);

void MP3_Adapt_Signal(int16_t *src, uint16_t *dst, uint16_t cnt, uint8_t volumen);

bool DAC_Wrapper_Is_Transfer_Done(void);

void DAC_Wrapper_Clear_Transfer_Done(void);

void DAC_Wrapper_Clear_Next_Buffer(void);

void DAC_Wrapper_Sleep(void);

void DAC_Wrapper_Wake_Up(void);

#endif /* DAC_WRAPER_H_ */
