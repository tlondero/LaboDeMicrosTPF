/**
 * @file    MK64FN1M0xxx12_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "debug_ifdefs.h"
#include "MK64F12.h"
#include "fsl_sysmpu.h"
#include "fsl_debug_console.h"
#include "sdmmc_config.h"
#include "fsl_smc.h"
#include "fsl_sd_disk.h"
#include "fsl_common.h"
#include "fsl_pit.h"
#include "SD_Detect_Wraper.h"
#include "mp3Decoder.h"
#include "ff.h"
#include "DAC_Wrapper.h"
#include "FS_explorer.h"
#include "event_handling/event_handler.h"
#include <fft.h>
#include "math_helper.h"
#include "button.h"

/**********************************************************************************************
 *                                          DEFINES                                           *
 **********************************************************************************************/

/**********************************************************************************************
 *                                    TYPEDEFS AND ENUMS                                      *
 **********************************************************************************************/

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
} player_context_t;

typedef struct {
	app_state_t appState;
	menu_state_t menuState;
	bool spectrogramEnable;
	uint8_t volume;
	char *currentFile;
	player_context_t playerContext;
} app_context_t;

/**********************************************************************************************
 *                        FUNCTION DECLARATION WITH LOCAL SCOPE                               *
 **********************************************************************************************/

void runMenu(event_t *events, app_context_t *context);
void runPlayer(event_t *events, app_context_t *context);
void switchAppState(app_state_t current, app_state_t target);
void resetAppContext(void);
void resetPlayerContext(void);
void switchOffKinetis(void);
void prepareForSwitchOn(void);
void prepareForSwitchOff(void);
char* concat(const char *s1, const char *s2);
int initDevice(void);
void cbackin(void);
void cbackout(void);

/**********************************************************************************************
 *                              VARIABLES WITH LOCAL SCOPE                                    *
 **********************************************************************************************/

static app_context_t appContext;

static short u_buffer_1[MP3_DECODED_BUFFER_SIZE];
static short u_buffer_2[MP3_DECODED_BUFFER_SIZE];

/**********************************************************************************************
 *                                         MAIN                                               *
 **********************************************************************************************/

int main(void) {

	initDevice(); /* Init device */
	prepareForSwitchOff();
	while (true) {
		event_t ev;

		EVHANDLER_GetEvents(&ev); /* Get events */

		switch (appContext.appState) {

		/***************/
		/*  OFF STATE  */
		/***************/

		case kAPP_STATE_OFF:
			switchOffKinetis(); /* Turn off */
			switchAppState(appContext.appState, kAPP_STATE_IDDLE); /* Go back to IDDLE */

			break;

			/***************/
			/* IDDLE STATE */
			/***************/

		case kAPP_STATE_IDDLE:
			if (SDWRAPPER_GetMounted() && SDWRAPPER_getJustIn()) {
				appContext.currentFile = FSEXP_exploreFS(FSEXP_ROOT_DIR); /* Explore filesystem */
#ifdef DEBUG_PRINTF_APP
				appContext.currentFile = FSEXP_getNext();
				printf("[App] Pointing currently to: %s\n",
						appContext.currentFile);
#endif
			}
			if (SDWRAPPER_getJustOut()) { /* If SD is removed */
				if (appContext.menuState == kAPP_MENU_FILESYSTEM) { /* and menu is exploring FS*/
					appContext.menuState = kAPP_MENU_MAIN; /* Go back to main menu */
				}
			}
			if (ev.btn_evs.off_on_button) {
				switchAppState(appContext.appState, kAPP_STATE_OFF);
			}
			if (SDWRAPPER_GetMounted() && SDWRAPPER_getSDInserted()) {
				if (ev.btn_evs.next_button) {
					if (FSEXP_getNext()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.prev_button) {
					if (FSEXP_getPrev()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.enter_button) {
#ifdef DEBUG_PRINTF_APP
					printf("[App] Opened %s\n", appContext.currentFile);
#endif
					if (FSEXP_openSelected()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Pointing currently to: %s\n",
							FSEXP_getMP3Path());
#endif
				}
				if (ev.btn_evs.back_button) {

					if (FSEXP_goBackDir()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Went back a directory\n");
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.pause_play_button) {
					appContext.playerContext.songResumed = true;
					switchAppState(appContext.appState, kAPP_STATE_PLAYING);
#ifdef DEBUG_PRINTF_APP
					printf("[App] Resumed playing.\n");
#endif
				}
				if (ev.fsexp_evs.play_music) {
					switchAppState(appContext.appState, kAPP_STATE_PLAYING);
					//TODO: Reiniciar las cosas porque es cancion nueva.
				}
			}
			runMenu(&ev, &appContext); /* Run menu in background */
			break;

			/***************/
			/*PLAYING STATE*/
			/***************/

		case kAPP_STATE_PLAYING:

			if (SDWRAPPER_getJustOut()) { /* If SD is removed */
				if (appContext.menuState == kAPP_MENU_FILESYSTEM) { /* and menu is exploring FS*/
					appContext.menuState = kAPP_MENU_MAIN; /* Go back to main menu */
					//TODO stop music, stop spectogram
					//...
					switchAppState(appContext.appState, kAPP_STATE_IDDLE); /* Return to iddle state */
					break;
				}
			}
			if (ev.btn_evs.off_on_button) {
				switchAppState(appContext.appState, kAPP_STATE_OFF);
			}
			if (SDWRAPPER_GetMounted() && SDWRAPPER_getSDInserted()) {
				if (ev.btn_evs.next_button) {
					if (FSEXP_getNext()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.prev_button) {
					if (FSEXP_getPrev()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.enter_button) {
#ifdef DEBUG_PRINTF_APP
					printf("[App] Opened %s\n", appContext.currentFile);
#endif
					if (FSEXP_openSelected()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.back_button) {

					if (FSEXP_goBackDir()) {
						appContext.currentFile = FSEXP_getFilename();
					}
#ifdef DEBUG_PRINTF_APP
					printf("[App] Went back a directory\n");
					printf("[App] Pointing currently to: %s\n",
							appContext.currentFile);
#endif
				}
				if (ev.btn_evs.pause_play_button) {
					appContext.playerContext.songPaused = true;
					switchAppState(appContext.appState, kAPP_STATE_IDDLE);
#ifdef DEBUG_PRINTF_APP
					printf("[App] Stopped playing.\n");
#endif
				}
				if (ev.fsexp_evs.play_music) {
#ifdef DEBUG_PRINTF_APP
					printf("[App] Switched to playing: %s\n",
							FSEXP_getMP3Path());
#endif
					//TODO: Reiniciar todo para reproducir la proxima cancion.
				}
				if (appContext.playerContext.songEnded) {
					switchAppState(appContext.appState, kAPP_STATE_IDDLE);
				}
			}

			runMenu(&ev, &appContext); /* Run menu in background */

			if (appContext.playerContext.songEnded) {
				appContext.playerContext.songEnded = false;
			} else {
				runPlayer(&ev, &appContext); /* Run player in background */
			}
			break;

		default:
			break;
		}
	}
	return 0;
}

/**********************************************************************************************
 *                        FUNCTION DEFINITIONS WITH LOCAL SCOPE                               *
 **********************************************************************************************/

int initDevice(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();

	/* Init FSL debug console. */
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	BOARD_InitDebugConsole();
#endif

#ifdef APP_KINETIS_LEDS
	/* Init kinetis ON/OFF leds */
	LED_RED_INIT(LOGIC_LED_ON);
	LED_GREEN_INIT(LOGIC_LED_OFF);
#endif

	/* Init Botonera */
	BUTTON_Init();

	/* Init Helix MP3 Decoder */
	MP3DecoderInit();

	/* Init SD wrapper */
	SDWRAPPER_Init(cbackin, cbackout); //Init PIT, SD
	SDWRAPPER_SetInterruptEnable(false);

	/* Init DAC */
	DAC_Wrapper_Init();		//Init DMAMUX, EDMA, PDB, DAC
	DAC_Wrapper_Loop(false);
	DAC_Wrapper_Start_Trigger();
	DAC_Wrapper_Sleep();

	/* Init event handlers */
	EVHANDLER_InitHandlers();

	/* Reset app context */
	resetAppContext();

	return true; //TODO agregar verificaciones al resto de los inits

}

void switchAppState(app_state_t current, app_state_t target) {
	switch (target) {
	case kAPP_STATE_OFF: /* Can come from IDDLE or PLAYING */
		if (current == kAPP_STATE_IDDLE) {
			prepareForSwitchOff();
			appContext.appState = target;
		} else if (current == kAPP_STATE_PLAYING) {
			DAC_Wrapper_Sleep();
			appContext.playerContext.firstDacTransmition = true;
			prepareForSwitchOff();
			appContext.appState = target;
		}
		break;
	case kAPP_STATE_IDDLE: /* Can come from OFF or PLAYING */
		if (current == kAPP_STATE_OFF) {
			prepareForSwitchOn();
			appContext.appState = target;
		} else if (current == kAPP_STATE_PLAYING) {
			//TODO: Stop player, spectrogram..
			DAC_Wrapper_Sleep();
			appContext.playerContext.firstDacTransmition = true;
			appContext.appState = target;
		}
		break;
	case kAPP_STATE_PLAYING: /* Can only come from IDDLE */
		if (current == kAPP_STATE_IDDLE) {
			//TODO: Spectrogram...

			if (!appContext.playerContext.songResumed) {

				resetPlayerContext();

				char *songName = FSEXP_getMP3Path();
				MP3LoadFile(&songName[1]);

#ifdef DEBUG_PRINTF_APP
				//TODO Calculo que vamos a tener que hacer algo con esta data
				printf("[App] Playing music...");
				if (MP3GetTagData(&appContext.playerContext.ID3Data)) {
					printf("\nSONG INFO\n");
					printf("TITLE: %s\n",
							appContext.playerContext.ID3Data.title);
					printf("ARTIST: %s\n",
							appContext.playerContext.ID3Data.artist);
					printf("ALBUM: %s\n",
							appContext.playerContext.ID3Data.album);
					printf("TRACK NUM: %s\n",
							appContext.playerContext.ID3Data.trackNum);
					printf("YEAR: %s\n", appContext.playerContext.ID3Data.year);
				}
#endif

				//Empiezo por el buffer 1
				appContext.playerContext.res = MP3GetDecodedFrame(
						(int16_t*) u_buffer_1,
						MP3_DECODED_BUFFER_SIZE,
						&appContext.playerContext.sampleCount, 0);

				uint16_t j;
				for (j = 0; j < appContext.playerContext.frameData.sampleCount;
						j++) {
					u_buffer_1[j] = (uint16_t) ((u_buffer_1[j] + 32768) * 4095
							/ 65535.0);
				}
				MP3_Set_Sample_Rate(appContext.playerContext.sr_,
						appContext.playerContext.ch_);

			} else {
				appContext.playerContext.songResumed = false;
				DAC_Wrapper_Clear_Data_Array();
			}
			DAC_Wrapper_Wake_Up();
			appContext.appState = target;
		}
		break;

	default:
		break;
	}
}

void runMenu(event_t *events, app_context_t *context) {
	//TODO: La fsm del menu
}
void runPlayer(event_t *events, app_context_t *context) {
	//TODO: Implementar aca el reproductor y el espectrograma (ver si no van a funcar a interrupciones tho)

	if (appContext.playerContext.songPaused) {
		DAC_Wrapper_Clear_Data_Array();
		appContext.playerContext.songPaused = false;
		DAC_Wrapper_Sleep();
	} else if (appContext.playerContext.res == MP3DECODER_NO_ERROR) {

		if (DAC_Wrapper_Is_Transfer_Done()
				|| appContext.playerContext.firstDacTransmition) { //Entro en la primera o cuando ya transmiti

			appContext.playerContext.firstDacTransmition = false;

			MP3GetLastFrameData(&(appContext.playerContext.frameData));

			//No debería cambiar el sample rate entre frame y frame
			//Pero si lo hace...
			if ((appContext.playerContext.sr_
					!= appContext.playerContext.frameData.sampleRate)
					|| (appContext.playerContext.ch_
							!= appContext.playerContext.frameData.channelCount)) {
				appContext.playerContext.sr_ =
						appContext.playerContext.frameData.sampleRate;
				appContext.playerContext.ch_ =
						appContext.playerContext.frameData.channelCount;
				MP3_Set_Sample_Rate(appContext.playerContext.sr_,
						appContext.playerContext.ch_);
			}

			DAC_Wrapper_Clear_Transfer_Done();

			uint16_t j;
			if (appContext.playerContext.using_buffer_1) {
				//Envio el buffer 1 al dac
				DAC_Wrapper_Set_Data_Array(&u_buffer_1,
						appContext.playerContext.frameData.sampleCount);
				DAC_Wrapper_Set_Next_Buffer(&u_buffer_2);

				//Cargo el y normalizo el buffer 2
				appContext.playerContext.res = MP3GetDecodedFrame(
						(int16_t*) u_buffer_2,
						MP3_DECODED_BUFFER_SIZE,
						&(appContext.playerContext.sampleCount), 0);

				for (j = 0; j < appContext.playerContext.frameData.sampleCount;
						j++) {
					u_buffer_2[j] = (uint16_t) ((u_buffer_2[j] + 32768) * 4095
							/ 65535.0);
				}
			} else {
				//Envio el buffer 2 al dac
				DAC_Wrapper_Set_Data_Array(&u_buffer_2,
						appContext.playerContext.frameData.sampleCount);
				DAC_Wrapper_Set_Next_Buffer(&u_buffer_1);

				//Cargo el y normalizo el buffer 1
				appContext.playerContext.res = MP3GetDecodedFrame(
						(int16_t*) u_buffer_1,
						MP3_DECODED_BUFFER_SIZE,
						&(appContext.playerContext.sampleCount), 0);
				for (j = 0; j < appContext.playerContext.frameData.sampleCount;
						j++) {
					u_buffer_1[j] = (uint16_t) ((u_buffer_1[j] + 32768) * 4095
							/ 65535.0);
				}
			}

			appContext.playerContext.using_buffer_1 =
					!appContext.playerContext.using_buffer_1;//Cambio el buffer al siguiente
		}
	} else if (appContext.playerContext.res == MP3DECODER_FILE_END) {
		appContext.playerContext.songEnded = true;
	}
}

char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void prepareForSwitchOff(void) {
	SDWRAPPER_SetCardPower(false);
	BOARD_SetPortsForWakeupSW2(true);
}

void prepareForSwitchOn(void) {
	BOARD_SetPortsForWakeupSW2(false);
	SDWRAPPER_SetCardPower(true);
	SDWRAPPER_SetInsertedFlag();
	SDWRAPPER_SetInterruptEnable(true);
}

void resetAppContext(void) {
	appContext.appState = kAPP_STATE_OFF;
	appContext.menuState = kAPP_MENU_MAIN;
	appContext.spectrogramEnable = false;
	appContext.volume = 50;
	appContext.currentFile = NULL;

	resetPlayerContext();
}

void resetPlayerContext(void) {
	appContext.playerContext.firstDacTransmition = true;
	appContext.playerContext.songPaused = false;
	appContext.playerContext.songResumed = false;
	appContext.playerContext.songEnded = false;
	appContext.playerContext.res = MP3DECODER_FILE_END;
	appContext.playerContext.sr_ = kMP3_44100Hz;
	appContext.playerContext.ch_ = kMP3_Stereo;
	appContext.playerContext.using_buffer_1 = true;
}

void switchOffKinetis(void) {
#ifdef DEBUG_PRINTF_APP
	printf("\nKinetis has stopped running.\n");
#endif

#ifdef APP_KINETIS_LEDS
	LED_RED_ON();
	LED_GREEN_OFF();
#endif

	//kinetisWakeupArmed = true; //TODO: volver a meter esto
	SMC_PreEnterStopModes();						//Pre entro al stop mode
	SMC_SetPowerModeStop(SMC, kSMC_PartialStop);	//Entro al stop mode
	SMC_PostExitStopModes();//Despues de apretar el SW2, se sigue en esta linea de codigo

#ifdef APP_KINETIS_LEDS
	LED_RED_OFF();
	LED_GREEN_ON();
#endif

#ifdef DEBUG_PRINTF_APP
	printf("\nKinetis is running.\n");
#endif
#ifdef DEBUG_PRINTF_APP
	printf("\nKinetis has gone iddle.\n");
#endif
}

void cbackin(void) {
#ifdef DEBUG_PRINTF_APP
	printf("[App] SD Card inserted.\r\n");
#endif

}

void cbackout(void) {
#ifdef DEBUG_PRINTF_APP
	printf("[App] SD Card removed.\r\n");
#endif
}
