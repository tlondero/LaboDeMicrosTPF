/***************************************************************************//**
 @file     FileName.c
 @brief
 @author   Guido Lambertucci
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "DAC_Wrapper.h"
#include "SD_Detect_Wraper.h"
#include "fsl_debug_console.h"
#include "FSM.h"
#include "FS_explorer.h"
#include "mp3Decoder.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define DEBUG_PRINTF_APP

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum {
	FS, VOL, EQ, NOCH
};
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/
void runPlayer(event_t *events, app_context_t *context);
/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
static uint16_t u_buffer_1[MP3_DECODED_BUFFER_SIZE];
static uint16_t u_buffer_2[MP3_DECODED_BUFFER_SIZE];

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void FSM_menu(event_t *ev, app_context_t *appContext) {

	if (appContext->menuState == kAPP_MENU_MAIN) {
		static uint8_t index = FS;

		if (ev->btn_evs.next_button) {
			index++;
			switch (index % 3) {
			case FS:
				printf("File System Explorer Menu\r\n");
				break;
			case VOL:
				printf("Volume Menu\r\n");
				break;
			case EQ:
				printf("Equalizer Menu\r\n");
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.prev_button) {
			index--;
			switch (index % 3) {
			case FS:
				printf("File System Explorer Menu\r\n");
				break;
			case VOL:
				printf("Volume Menu\r\n");
				break;
			case EQ:
				printf("Equalizer Menu\r\n");
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.enter_button) {
			switch (index % 3) {
			case FS:
				printf("File System Explorer Menu opened\r\n");
				appContext->currentFile = FSEXP_exploreFS(FSEXP_ROOT_DIR);
				appContext->menuState = kAPP_MENU_FILESYSTEM;
				break;
			case VOL:
				printf("Volume Menu opened\r\n");
				appContext->menuState = kAPP_MENU_VOLUME;
				break;
			case EQ:
				printf("Equalizer Menu opened\r\n");
				appContext->menuState = kAPP_MENU_EQUALIZER;
				break;
			default:
				break;
			}
		}

	} else if (appContext->menuState == kAPP_MENU_FILESYSTEM) {
		if (ev->btn_evs.off_on_button) {
			switchAppState(appContext->appState, kAPP_STATE_OFF);
		}
		if (SDWRAPPER_GetMounted() && SDWRAPPER_getSDInserted()) {
			if (ev->btn_evs.next_button) {
				if (FSEXP_getNext()) {
					appContext->currentFile = FSEXP_getFilename();
				}
#ifdef DEBUG_PRINTF_APP
				printf("[App] Pointing currently to: %s\n",
						appContext->currentFile);
#endif
			}
			if (ev->btn_evs.prev_button) {
				if (FSEXP_getPrev()) {
					appContext->currentFile = FSEXP_getFilename();
				}
#ifdef DEBUG_PRINTF_APP
				printf("[App] Pointing currently to: %s\n",
						appContext->currentFile);
#endif
			}
			if (ev->btn_evs.enter_button) {
#ifdef DEBUG_PRINTF_APP
				printf("[App] Opened %s\n", appContext->currentFile);
#endif
				if (FSEXP_openSelected()) {
					appContext->currentFile = FSEXP_getFilename();
				}
#ifdef DEBUG_PRINTF_APP
				printf("[App] Pointing currently to: %s\n",
						appContext->currentFile);
#endif
			}
			if (ev->btn_evs.back_button) {

				if (FSEXP_goBackDir()) {
					appContext->currentFile = FSEXP_getFilename();

#ifdef DEBUG_PRINTF_APP
					printf("[App] Went back a directory\n");
					printf("[App] Pointing currently to: %s\n",
							appContext->currentFile);
#endif
				} else {
					appContext->menuState = kAPP_MENU_MAIN;
				}
			}

			if (ev->fsexp_evs.play_music) {
#ifdef DEBUG_PRINTF_APP
				printf("[App] Switched to playing: %s\n", FSEXP_getMP3Path());
#endif
				appContext->playerContext.firstDacTransmition = true;
				appContext->playerContext.songPaused = false;
				appContext->playerContext.songResumed = false;
				appContext->playerContext.songEnded = false;
				appContext->playerContext.res = MP3DECODER_FILE_END;
				appContext->playerContext.sr_ = kMP3_44100Hz;
				appContext->playerContext.ch_ = kMP3_Stereo;
				appContext->playerContext.using_buffer_1 = true;
				//TODO: Reiniciar todo para reproducir la proxima cancion.
				switchAppState(appContext->appState, kAPP_STATE_PLAYING);
			}

		}

	} else if (appContext->menuState == kAPP_MENU_VOLUME) {
		if (ev->btn_evs.next_button) {
			if (appContext->volume < 30) {
				appContext->volume++;
			}

#ifdef DEBUG_PRINTF_APP
			printf("[App] Volume set to: %d\n", appContext->volume);
#endif
		}

		else if (ev->btn_evs.prev_button) {
			if (appContext->volume > 0) {
				appContext->volume--;
			}

#ifdef DEBUG_PRINTF_APP
			printf("[App] Volume set to: %d\n", appContext->volume);
#endif
		} else if (ev->btn_evs.back_button) {
			appContext->menuState = kAPP_MENU_MAIN;
		}
	}
}

uint16_t* getbuffer1(void) {
	return u_buffer_1;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/
void runPlayer(event_t *events, app_context_t *appContext) {
//TODO: Implementar aca el reproductor y el espectrograma (ver si no van a funcar a interrupciones tho)

	if (appContext->playerContext.songPaused) {
		DAC_Wrapper_Clear_Data_Array();
		DAC_Wrapper_Clear_Next_Buffer();
		DAC_Wrapper_Sleep();
	} else if (appContext->playerContext.res == MP3DECODER_NO_ERROR) {

		if (DAC_Wrapper_Is_Transfer_Done()
				|| appContext->playerContext.firstDacTransmition) { //Entro en la primera o cuando ya transmiti

			appContext->playerContext.firstDacTransmition = false;

			MP3GetLastFrameData(&(appContext->playerContext.frameData));

			//No debería cambiar el sample rate entre frame y frame
			//Pero si lo hace...
			if ((appContext->playerContext.sr_
					!= appContext->playerContext.frameData.sampleRate)
					|| (appContext->playerContext.ch_
							!= appContext->playerContext.frameData.channelCount)) {
				appContext->playerContext.sr_ =
						appContext->playerContext.frameData.sampleRate;
				appContext->playerContext.ch_ =
						appContext->playerContext.frameData.channelCount;
				MP3_Set_Sample_Rate(appContext->playerContext.sr_,
						appContext->playerContext.ch_);
			}

			DAC_Wrapper_Clear_Transfer_Done();

			if (appContext->playerContext.using_buffer_1) {
				//Envio el buffer 1 al dac
				DAC_Wrapper_Set_Data_Array(&u_buffer_1,
						appContext->playerContext.frameData.sampleCount);
				DAC_Wrapper_Set_Next_Buffer(&u_buffer_2);

				//Cargo el y normalizo el buffer 2
				appContext->playerContext.res = MP3GetDecodedFrame(
						(int16_t*) u_buffer_2,
						MP3_DECODED_BUFFER_SIZE,
						&(appContext->playerContext.sampleCount), 0);

				MP3_Adapt_Signal((int16_t*) u_buffer_2, u_buffer_2,
						appContext->playerContext.sampleCount,
						appContext->volume);
			} else {
				//Envio el buffer 2 al dac
				DAC_Wrapper_Set_Data_Array(&u_buffer_2,
						appContext->playerContext.frameData.sampleCount);
				DAC_Wrapper_Set_Next_Buffer(&u_buffer_1);

				//Cargo el y normalizo el buffer 1
				appContext->playerContext.res = MP3GetDecodedFrame(
						(int16_t*) u_buffer_1,
						MP3_DECODED_BUFFER_SIZE,
						&(appContext->playerContext.sampleCount), 0);
				MP3_Adapt_Signal((int16_t*) u_buffer_1, u_buffer_1,
						appContext->playerContext.sampleCount,
						appContext->volume);
			}

			appContext->playerContext.using_buffer_1 =
					!appContext->playerContext.using_buffer_1;//Cambio el buffer al siguiente
		}
	} else if (appContext->playerContext.res == MP3DECODER_FILE_END) {
		appContext->playerContext.songEnded = true;
	}
}
/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

