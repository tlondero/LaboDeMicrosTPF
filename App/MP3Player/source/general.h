/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   Guido Lambertucci
 ******************************************************************************/
#ifndef GENERAL_H_
#define GENERAL_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "mp3Decoder.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


typedef enum {
	kAPP_STATE_OFF, kAPP_STATE_IDDLE, kAPP_STATE_PLAYING
} app_state_t;

typedef enum {
	kAPP_MENU_MAIN,
	kAPP_MENU_FILESYSTEM,
	kAPP_MENU_VOLUME,
	kAPP_MENU_EQUALIZER,
	kAPP_MENU_SPECTROGRAM
} menu_state_t;

typedef struct {
	bool firstDacTransmition;
	bool songPaused;
	bool songResumed;
	bool songEnded;
	mp3_decoder_result_t res;
	uint16_t sr_;	//Default config 4 mp3 stereo
	uint8_t ch_;
	bool using_buffer_1;
	mp3_decoder_frame_data_t frameData;
	mp3_decoder_tag_data_t ID3Data;
	uint16_t sampleCount;
	uint8_t volume;
} player_context_t;

typedef struct {
	app_state_t appState;
	menu_state_t menuState;
	bool spectrogramEnable;
	uint8_t volume;
	char *currentFile;
	player_context_t playerContext;
} app_context_t;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  --
 * @param  --
 * @return --
*/

#endif /* GENERAL_H_ */
