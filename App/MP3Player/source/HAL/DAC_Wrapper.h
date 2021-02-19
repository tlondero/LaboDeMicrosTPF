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

/**
 * @brief Initializes the modules: DAC, EDMA, DMA and PDB
 */
void DAC_Wrapper_Init(void);

/**
 * @brief Sets array to send to DAC
 * @param
 * 		@newDataArray -- New data
 * 		@newSizeOf -- Size of data array
*/
void DAC_Wrapper_Set_Data_Array(void *newDataArray, uint32_t newSizeOf);

/**
 * @brief Set next buffer to send to DAC in order to avoid delays
 * @param
 * 		@forcedBackUp -- Next buffer array
 */
void DAC_Wrapper_Set_Next_Buffer(void *forcedBackUp);

/**
 * @brief Sets only zeros to DAC
*/
void DAC_Wrapper_Clear_Data_Array(void);

/**
 * @brief Initializes PDB trigger for DAC
*/
void DAC_Wrapper_Start_Trigger(void);		//Start trigger signal to DAC

/**
 * @brief Determinates if the it should be send the same buffer to DAC after sending all data
*/
void DAC_Wrapper_Loop(bool status);

/**
 * @brief Sets PDB parameters to modify DAC Fs
 * @param
 * 		@mod_val -- Modulus
 * 		@mult_fact -- Multiplication factor
 * 		@prescaler -- Prescaler
*/
void DAC_Wrapper_PDB_Config(uint32_t mod_val,
		pdb_divider_multiplication_factor_t mult_fact,
		pdb_prescaler_divider_t prescaler);

/**
 * @brief Sets DAC sample rate for standar mp3 calues
 * @param
 * 		@sr -- Sample rate
 * 		@ch -- Channel: MONO / STEREO
 *
 * @return true if the the settings are from standar mp3
*/
bool MP3_Set_Sample_Rate(uint16_t sr, uint8_t ch);

/**
 * @brief Modulates signal from mp3 data to DAC
 * @param
 * 		@src -- source buffer
 * 		@dst -- destiny buffer
 *		@cnt -- size of buffer
 *		@volume -- volume
 *
 * @return true if it was succesfull the initialization.
*/
void MP3_Adapt_Signal(int16_t *src, uint16_t *dst, uint16_t cnt, uint8_t volumen);

/**
 * @brief Check if the buffer has already send all data from buffer
 *
 * @return true if it ended all data
*/
bool DAC_Wrapper_Is_Transfer_Done(void);

/**
 * @brief Clears the transfer done flag
*/
void DAC_Wrapper_Clear_Transfer_Done(void);

/**
 * @brief Clears the next buffer
*/
void DAC_Wrapper_Clear_Next_Buffer(void);

/**
 * @brief Stops DAC
*/
void DAC_Wrapper_Sleep(void);

/**
 * @brief Starts DAC
*/
void DAC_Wrapper_Wake_Up(void);

#endif /* DAC_WRAPER_H_ */
