/***************************************************************************//**
 @file     FSM.c
 @brief
 @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../HAL/DAC_Wrapper.h"
#include "../HAL/SD_Detect_Wraper.h"
#include "fsl_debug_console.h"
#include "FSM.h"
#include "FS_explorer.h"
#include "../HAL/LED_Matrix.h"
#include "mp3Decoder.h"
#include "fsl_uart.h"
#include "debug_ifdefs.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum {
	FS, EQ, SPECT, SUBMENU_CANT
};
enum {
	OFF,
	CLASSIC,
	CLUB,
	DANCE,
	BASS,
	BASS_AND_TREBLE,
	TREBLE,
	HEADSET,
	HALL,
	LIVE,
	PARTY,
	POP,
	REGGAE,
	ROCK,
	SKA,
	SOFT,
	SOFT_ROCK,
	TECHNO,
	CANT_PRESETS
};
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/
/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
static uint16_t u_buffer_1[MP3_DECODED_BUFFER_SIZE];
static uint16_t u_buffer_2[MP3_DECODED_BUFFER_SIZE];
static char packPath[MAX_PATH_LENGHT + 5];
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void FSM_menu(event_t *ev, app_context_t *appContext) {

	if (appContext->menuState == kAPP_MENU_MAIN) {
		static uint8_t index = FS;
		if (ev->btn_evs.next_button) {
			index++;
			switch (index % SUBMENU_CANT) {
			case FS:
#ifdef DEBUG_PRINTF_APP
				printf("File System Explorer Menu\r\n");
#else
				UART_WriteBlocking(UART0, "00File System Explorer Menu\r\n",
						30);
#endif
				break;

			case EQ:
#ifdef DEBUG_PRINTF_APP
				printf("Equalizer Menu\r\n");
#else
				UART_WriteBlocking(UART0, "00Equalizer Menu\r\n", 19);

#endif
				break;
			case SPECT:
#ifdef DEBUG_PRINTF_APP
				if (appContext->spectrogramEnable) {
					printf("Spectrogram on\r\n");
				} else {
					printf("Spectrogram off\r\n");
				}
#else
				if (appContext->spectrogramEnable) {
					UART_WriteBlocking(UART0, "00Spectrogram on\r\n", 19);

				} else {
					UART_WriteBlocking(UART0, "00Spectrogram off\r\n", 20);
				}
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.prev_button) {
			index--;
			switch (index % SUBMENU_CANT) {
			case FS:
#ifdef DEBUG_PRINTF_APP
				printf("File System Explorer Menu\r\n");
#else
				UART_WriteBlocking(UART0, "00File System Explorer Menu\r\n",
						30);
#endif
				break;
			case EQ:
#ifdef DEBUG_PRINTF_APP
				printf("Equalizer Menu\r\n");
#else
				UART_WriteBlocking(UART0, "00Equalizer Menu\r\n", 19);
#endif
				break;
			case SPECT:
#ifdef DEBUG_PRINTF_APP
				if (appContext->spectrogramEnable) {
					printf("Spectrogram on\r\n");
				} else {
					printf("Spectrogram off\r\n");
				}
#else
				if (appContext->spectrogramEnable) {
					UART_WriteBlocking(UART0, "00Spectrogram on\r\n", 19);

				} else {
					UART_WriteBlocking(UART0, "00Spectrogram off\r\n", 20);
				}
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.enter_button) {
			switch (index % SUBMENU_CANT) {
			case FS:
				appContext->currentFile = FSEXP_exploreFS(FSEXP_ROOT_DIR);
#ifdef DEBUG_PRINTF_APP
				printf("File System Explorer Menu opened\r\n");
				printf("Currently pointing to: %s\r\n",
						appContext->currentFile);
#else
				packPath[0] = '0';
				packPath[1] = '6';
				copyFname(&(packPath[2]), appContext->currentFile);
				uint16_t i = 0;
				while (packPath[i] != '\0') {
					i++;
				}
				packPath[i++] = '\r';
				packPath[i++] = '\n';
				packPath[i++] = '\0';
				UART_WriteBlocking(UART0, packPath, i);


#endif
				appContext->menuState = kAPP_MENU_FILESYSTEM;
				break;

			case EQ:
#ifdef DEBUG_PRINTF_APP
				printf("Equalizer Menu opened\r\n");
#endif
				appContext->menuState = kAPP_MENU_EQUALIZER;
				break;
			case SPECT:
				appContext->spectrogramEnable = !appContext->spectrogramEnable;
				if (appContext->spectrogramEnable) {
					LEDMATRIX_Resume();
#ifdef DEBUG_PRINTF_APP
					printf("Spectrogram on\r\n");
#else
					UART_WriteBlocking(UART0, "00Spectrogram on\r\n", 19);
#endif
				} else {
					LEDMATRIX_Pause();
#ifdef DEBUG_PRINTF_APP
					printf("Spectrogram off\r\n");
#else
					UART_WriteBlocking(UART0, "00Spectrogram off\r\n", 20);
#endif
				}
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.back_button) {

		}
	} else if (appContext->menuState == kAPP_MENU_FILESYSTEM) {
		if (SDWRAPPER_GetMounted() && SDWRAPPER_getSDInserted()) {
			if (ev->btn_evs.next_button) {
				if (FSEXP_getNext()) {
					appContext->currentFile = FSEXP_getFilename();
				}
#ifdef DEBUG_PRINTF_APP
				printf("[App] Pointing currently to: %s\n",
						appContext->currentFile);
#else
				packPath[0] = '0';
				packPath[1] = '6';
				copyFname(&(packPath[2]), appContext->currentFile);
				uint16_t i = 0;
				while (packPath[i] != '\0') {
					i++;
				}
				packPath[i++] = '\r';
				packPath[i++] = '\n';
				packPath[i++] = '\0';
				UART_WriteBlocking(UART0, packPath, i);

#endif
			} else if (ev->btn_evs.prev_button) {
				if (FSEXP_getPrev()) {
					appContext->currentFile = FSEXP_getFilename();
				}
#ifdef DEBUG_PRINTF_APP
				printf("[App] Pointing currently to: %s\n",
						appContext->currentFile);
#else
				packPath[0] = '0';
				packPath[1] = '6';
				copyFname(&(packPath[2]), appContext->currentFile);
				uint16_t i = 0;
				while (packPath[i] != '\0') {
					i++;
				}
				packPath[i++] = '\r';
				packPath[i++] = '\n';
				packPath[i++] = '\0';
				UART_WriteBlocking(UART0, packPath, i);

#endif
			} else if (ev->btn_evs.enter_button) {
#ifdef DEBUG_PRINTF_APP
				printf("[App] Opened %s\n", appContext->currentFile);
#endif
				if (FSEXP_openSelected()) {
					appContext->currentFile = FSEXP_getFilename();

				}
#ifdef DEBUG_PRINTF_APP
				printf("[App] Pointing currently to: %s\n",
						appContext->currentFile);
#else
				packPath[0] = '0';
				packPath[1] = '6';
				copyFname(&(packPath[2]), appContext->currentFile);
				uint16_t i = 0;
				while (packPath[i] != '\0') {
					i++;
				}
				packPath[i++] = '\r';
				packPath[i++] = '\n';
				packPath[i++] = '\0';
				UART_WriteBlocking(UART0, packPath, i);

#endif
			} else if (ev->btn_evs.back_button) {
				if (FSEXP_goBackDir()) {
					appContext->currentFile = FSEXP_getFilename();

#ifdef DEBUG_PRINTF_APP
					printf("[App] Went back a directory\n");
					printf("[App] Pointing currently to: %s\n",
							appContext->currentFile);
#else
					packPath[0] = '0';
					packPath[1] = '6';
					copyFname(&(packPath[2]), appContext->currentFile);
					uint16_t i = 0;
					while (packPath[i] != '\0') {
						i++;
					}
					packPath[i++] = '\r';
					packPath[i++] = '\n';
					packPath[i++] = '\0';
					UART_WriteBlocking(UART0, packPath, i);
#endif
				} else {
					appContext->menuState = kAPP_MENU_MAIN;
#ifdef DEBUG_PRINTF_APP
					printf("Main menu\r\n");
#endif
				}
			} else if (ev->fsexp_evs.play_music) {
#ifdef DEBUG_PRINTF_APP
				printf("[App] Switched to playing: %s\n", FSEXP_getMP3Path());
#endif
				DAC_Wrapper_Clear_Data_Array();
				DAC_Wrapper_Clear_Next_Buffer();
				appContext->playerContext.firstDacTransmition = true;
				appContext->playerContext.songPaused = false;
				appContext->playerContext.songResumed = false;
				appContext->playerContext.songEnded = false;
				appContext->playerContext.res = MP3DECODER_FILE_END;
				appContext->playerContext.sr_ = kMP3_44100Hz;
				appContext->playerContext.ch_ = kMP3_Stereo;
				appContext->playerContext.using_buffer_1 = true;
				switchAppState(appContext->appState, kAPP_STATE_PLAYING);
			}
		}
	}

	else if (appContext->menuState == kAPP_MENU_EQUALIZER) {
		static uint8_t index = OFF;
		if (ev->btn_evs.next_button) {
			index++;
			switch (index % CANT_PRESETS) {
			//OFF,CLASSIC,CLUB,DANCE,BASS,BASS_AND_TREBLE , TREBLE, HEADSET,HALL,LIVE,PARTY,POP,REGGAE,ROCK,SKA,SOFT,SOFT_ROCK,TECHNO
			case OFF:
#ifdef DEBUG_PRINTF_APP
				printf("OFF preset\r\n");
#else
				UART_WriteBlocking(UART0, "11OFF\r\n", 8);
#endif
				break;
			case CLASSIC:
#ifdef DEBUG_PRINTF_APP
				printf("CLASSIC preset\r\n");
#else
				UART_WriteBlocking(UART0, "11CLASSIC\r\n", 12);
#endif
				break;
			case CLUB:
#ifdef DEBUG_PRINTF_APP
							printf("CLUB preset\r\n");
			#else
				UART_WriteBlocking(UART0, "11CLUB\r\n", 9);
#endif
				break;
			case DANCE:
#ifdef DEBUG_PRINTF_APP
				printf("DANCE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11DANCE\r\n", 10);
#endif
				break;
			case BASS:
#ifdef DEBUG_PRINTF_APP
				printf("BASS preset\r\n");
#else
				UART_WriteBlocking(UART0, "11BASS\r\n", 9);
#endif
				break;
			case BASS_AND_TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("BASS_AND_TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11BASS_AND_TREBLE\r\n", 20);
#endif
				break;
			case TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11TREBLE\r\n", 11);
#endif
				break;
			case HEADSET:
#ifdef DEBUG_PRINTF_APP
				printf("HEADSET preset\r\n");
#else
				UART_WriteBlocking(UART0, "11HEADSET\r\n", 12);
#endif
				break;
			case HALL:
#ifdef DEBUG_PRINTF_APP
				printf("HALL preset\r\n");
#else
				UART_WriteBlocking(UART0, "11HALL\r\n", 9);
#endif
				break;
			case LIVE:
#ifdef DEBUG_PRINTF_APP
				printf("LIVE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11LIVE\r\n", 9);
#endif
				break;
			case PARTY:
#ifdef DEBUG_PRINTF_APP
				printf("PARTY preset\r\n");
#else
				UART_WriteBlocking(UART0, "11PARTY\r\n", 10);
#endif
				break;
			case POP:
#ifdef DEBUG_PRINTF_APP
				printf("POP preset\r\n");
#else
				UART_WriteBlocking(UART0, "11POP\r\n", 8);
#endif
				break;
			case REGGAE:
#ifdef DEBUG_PRINTF_APP
				printf("REGGAE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11REGGAE\r\n", 11);
#endif
				break;
			case ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, "11ROCK\r\n", 9);
#endif
				break;
			case SKA:
#ifdef DEBUG_PRINTF_APP
				printf("SKA preset\r\n");
#else
				UART_WriteBlocking(UART0, "11SKA\r\n", 8);
#endif
				break;
			case SOFT:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT preset\r\n");
#else
				UART_WriteBlocking(UART0, "11SOFT\r\n", 9);
#endif
				break;
			case SOFT_ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT_ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, "11SOFT_ROCK\r\n", 14);
#endif
				break;
			case TECHNO:
#ifdef DEBUG_PRINTF_APP
				printf("TECHNO preset\r\n");
#else
				UART_WriteBlocking(UART0, "11TECHNO\r\n", 11);
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.prev_button) {
			index--;
			switch (index % CANT_PRESETS) {
			//OFF,CLASSIC,CLUB,DANCE,BASS,BASS_AND_TREBLE , TREBLE, HEADSET,HALL,LIVE,PARTY,POP,REGGAE,ROCK,SKA,SOFT,SOFT_ROCK,TECHNO
			case OFF:
#ifdef DEBUG_PRINTF_APP
				printf("OFF preset\r\n");
#else
				UART_WriteBlocking(UART0, "11OFF\r\n", 8);
#endif
				break;
			case CLASSIC:
#ifdef DEBUG_PRINTF_APP
				printf("CLASSIC preset\r\n");
#else
				UART_WriteBlocking(UART0, "11CLASSIC\r\n", 12);
#endif
				break;
			case CLUB:
#ifdef DEBUG_PRINTF_APP
							printf("CLUB preset\r\n");
			#else
				UART_WriteBlocking(UART0, "11CLUB\r\n", 9);
#endif
				break;
			case DANCE:
#ifdef DEBUG_PRINTF_APP
				printf("DANCE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11DANCE\r\n", 10);
#endif
				break;
			case BASS:
#ifdef DEBUG_PRINTF_APP
				printf("BASS preset\r\n");
#else
				UART_WriteBlocking(UART0, "11BASS\r\n", 9);
#endif
				break;
			case BASS_AND_TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("BASS_AND_TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11BASS_AND_TREBLE\r\n", 20);
#endif
				break;
			case TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11TREBLE\r\n", 11);
#endif
				break;
			case HEADSET:
#ifdef DEBUG_PRINTF_APP
				printf("HEADSET preset\r\n");
#else
				UART_WriteBlocking(UART0, "11HEADSET\r\n", 12);
#endif
				break;
			case HALL:
#ifdef DEBUG_PRINTF_APP
				printf("HALL preset\r\n");
#else
				UART_WriteBlocking(UART0, "11HALL\r\n", 9);
#endif
				break;
			case LIVE:
#ifdef DEBUG_PRINTF_APP
				printf("LIVE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11LIVE\r\n", 9);
#endif
				break;
			case PARTY:
#ifdef DEBUG_PRINTF_APP
				printf("PARTY preset\r\n");
#else
				UART_WriteBlocking(UART0, "11PARTY\r\n", 10);
#endif
				break;
			case POP:
#ifdef DEBUG_PRINTF_APP
				printf("POP preset\r\n");
#else
				UART_WriteBlocking(UART0, "11POP\r\n", 8);
#endif
				break;
			case REGGAE:
#ifdef DEBUG_PRINTF_APP
				printf("REGGAE preset\r\n");
#else
				UART_WriteBlocking(UART0, "11REGGAE\r\n", 11);
#endif
				break;
			case ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, "11ROCK\r\n", 9);
#endif
				break;
			case SKA:
#ifdef DEBUG_PRINTF_APP
				printf("SKA preset\r\n");
#else
				UART_WriteBlocking(UART0, "11SKA\r\n", 8);
#endif
				break;
			case SOFT:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT preset\r\n");
#else
				UART_WriteBlocking(UART0, "11SOFT\r\n", 9);
#endif
				break;
			case SOFT_ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT_ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, "11SOFT_ROCK\r\n", 14);
#endif
				break;
			case TECHNO:
#ifdef DEBUG_PRINTF_APP
				printf("TECHNO preset\r\n");
#else
				UART_WriteBlocking(UART0, "11TECHNO\r\n", 11);
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.enter_button) {
			switch (index % CANT_PRESETS) {
			//OFF,CLASSIC,CLUB,DANCE,BASS,BASS_AND_TREBLE , TREBLE, HEADSET,HALL,LIVE,PARTY,POP,REGGAE,ROCK,SKA,SOFT,SOFT_ROCK,TECHNO
			case OFF:
#ifdef DEBUG_PRINTF_APP
				printf("OFF preset Selected\r\n");
#endif
				break;
			case CLASSIC:
#ifdef DEBUG_PRINTF_APP
				printf("CLASSIC preset Selected\r\n");
#endif
				/*
				 *TODO ADD PRESETS HERE
				 * */
				break;
			case CLUB:
#ifdef DEBUG_PRINTF_APP
				printf("CLUB preset Selected\r\n");
#endif
				break;
			case DANCE:
#ifdef DEBUG_PRINTF_APP
				printf("DANCE preset Selected\r\n");
#endif
				break;
			case BASS:
#ifdef DEBUG_PRINTF_APP
				printf("BASS preset Selected\r\n");
#endif
				break;
			case BASS_AND_TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("BASS_AND_TREBLE preset Selected\r\n");
#endif
				break;
			case TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("TREBLE preset Selected\r\n");
#endif
				break;
			case HEADSET:
#ifdef DEBUG_PRINTF_APP
				printf("HEADSET preset Selected\r\n");
#endif
				break;
			case HALL:
#ifdef DEBUG_PRINTF_APP
				printf("HALL preset Selected\r\n");
#endif
				break;
			case LIVE:
#ifdef DEBUG_PRINTF_APP
				printf("LIVE preset Selected\r\n");
#endif
				break;
			case PARTY:
#ifdef DEBUG_PRINTF_APP
				printf("PARTY preset Selected\r\n");
#endif
				break;
			case POP:
#ifdef DEBUG_PRINTF_APP
				printf("POP preset Selected\r\n");
#endif
				break;
			case REGGAE:
#ifdef DEBUG_PRINTF_APP
				printf("REGGAE preset Selected\r\n");
#endif
				break;
			case ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("ROCK preset Selected\r\n");
#endif
				break;
			case SKA:
#ifdef DEBUG_PRINTF_APP
				printf("SKA preset Selected\r\n");
#endif
				break;
			case SOFT:
			#ifdef DEBUG_PRINTF_APP
							printf("SOFT preset Selected\r\n");
			#endif
							break;
			case SOFT_ROCK:
			#ifdef DEBUG_PRINTF_APP
							printf("SOFT_ROCK preset Selected\r\n");
			#endif
							break;
			case TECHNO:
			#ifdef DEBUG_PRINTF_APP
							printf("TECHNO preset Selected\r\n");
			#endif
							break;

			default:
				break;
			}
		} else if (ev->btn_evs.back_button) {
			appContext->menuState = kAPP_MENU_MAIN;
#ifdef DEBUG_PRINTF_APP
			printf("Main menu\r\n");
#endif
		}
	}
}

uint16_t* getbuffer1(void) {
	return u_buffer_1;
}

void runPlayer(event_t *events, app_context_t *appContext) {

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
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/

