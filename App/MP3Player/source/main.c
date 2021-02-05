/**
 * @file    MK64FN1M0xxx12_Project.c
 * @brief   Application entry point.
 */

#include <stdio.h>
#include "board.h"

#include "event_handling/event_handler.h"

#include "debug_ifdefs.h"

#include "fsl_pit.h"
#include "fsl_uart.h"

#include "../HAL/SD_Detect_Wraper.h"
#include "../HAL/DAC_Wrapper.h"
#include "FS_explorer.h"

#include <fft.h>

#include "../HAL/button.h"
#include "encoder.h"
#include "FSM.h"
#include "general.h"
#include "../HAL/LED_Matrix.h"

/**********************************************************************************************
 *                                          DEFINES                                           *
 **********************************************************************************************/

/**********************************************************************************************
 *                                    TYPEDEFS AND ENUMS                                      *
 **********************************************************************************************/

/**********************************************************************************************
 *                        FUNCTION DECLARATION WITH LOCAL SCOPE                               *
 **********************************************************************************************/
void adaptFFT(float32_t *src, float32_t *dst, uint16_t cnt);
void runMenu(event_t *events, app_context_t *context);

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
static char volString[7];
static char id3Buffer[ID3_MAX_FIELD_SIZE + 2];
/**********************************************************************************************
 *                                         MAIN                                               *
 **********************************************************************************************/

int main(void) {

	initDevice(); /* Init device */

	//Debug, dejar porque no jode..
	LEDMATRIX_SetLed(1, 2, 0, 0, 100);
	LEDMATRIX_SetLed(1, 5, 0, 0, 100);
	LEDMATRIX_SetLed(4, 1, 0, 0, 100);
	LEDMATRIX_SetLed(5, 2, 0, 0, 100);
	LEDMATRIX_SetLed(5, 3, 0, 0, 100);
	LEDMATRIX_SetLed(5, 4, 0, 0, 100);
	LEDMATRIX_SetLed(5, 5, 0, 0, 100);
	LEDMATRIX_SetLed(4, 6, 0, 0, 100);
	LEDMATRIX_Enable();
	LEDMATRIX_Pause();
	/*vol string initialize*/
	volString[0] = '0';
	volString[1] = '9';
	volString[2] = '1';
	volString[3] = '5';
	volString[4] = '\r';
	volString[5] = '\n';
	volString[6] = '\0';

	/* ID3 initialize */
	id3Buffer[0] = '0';

	prepareForSwitchOff();
	while (true) {
		event_t ev = { 0 };
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
			if (SDWRAPPER_getJustOut()) { /* If SD is removed */
				if (appContext.menuState == kAPP_MENU_FILESYSTEM) { /* and menu is exploring FS*/
					appContext.menuState = kAPP_MENU_MAIN; /* Go back to main menu */
				}
			} else if (ev.btn_evs.off_on_button) {
				switchAppState(appContext.appState, kAPP_STATE_OFF);
			} else if (ev.btn_evs.pause_play_button) {
				appContext.playerContext.songResumed = true;
				switchAppState(appContext.appState, kAPP_STATE_PLAYING);
#ifdef DEBUG_PRINTF_APP
				printf("[App] Resumed playing.\n");
				#else
				UART_WriteBlocking(UART0, "08P\r\n", 6);
#endif
			} else if (ev.encoder_evs.back_button && appContext.volume > 0) {
				appContext.volume--;

#ifdef DEBUG_PRINTF_APP
printf("[App] Volume decreased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, volString, 7);
#endif
			} else if (ev.encoder_evs.next_button && appContext.volume < 30) {
				appContext.volume++;
#ifdef DEBUG_PRINTF_APP
printf("[App] Volume increased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, volString, 7);
#endif
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
				}
			}
			if (ev.btn_evs.pause_play_button) {
				appContext.playerContext.songPaused = true;
				switchAppState(appContext.appState, kAPP_STATE_IDDLE);
#ifdef DEBUG_PRINTF_APP
				printf("[App] Stopped playing.\n");
#else
				UART_WriteBlocking(UART0, "08S\r\n", 6);
#endif
			} else if (ev.btn_evs.off_on_button) {
				switchAppState(appContext.appState, kAPP_STATE_OFF);
			} else if (appContext.playerContext.songEnded) {

				appContext.playerContext.songEnded = false;

				if (FSEXP_getNext()) {
					appContext.currentFile = FSEXP_getFilename();

					char *myPackPath = GetPackPath();

					myPackPath[0] = '0';
					myPackPath[1] = '6';
					copyFname(&(myPackPath[2]), appContext.currentFile);
					uint16_t i = 0;
					while (myPackPath[i] != '\0') {
						i++;
					}
					myPackPath[i++] = '\r';
					myPackPath[i++] = '\n';
					myPackPath[i++] = '\0';
					UART_WriteBlocking(UART0, myPackPath, i);

					if ((!(FSEXP_isdir())) && FSEXP_openSelected()) {
						appContext.currentFile = FSEXP_getFilename();
					} else {
						switchAppState(appContext.appState, kAPP_STATE_IDDLE);
					}
				} else {
					switchAppState(appContext.appState, kAPP_STATE_IDDLE);
				}
			} else if (ev.encoder_evs.back_button && appContext.volume > 0) {
				appContext.volume--;
#ifdef DEBUG_PRINTF_APP
printf("[App] Volume decreased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, volString, 7);
#endif
			} else if (ev.encoder_evs.next_button && appContext.volume < 30) {
				appContext.volume++;
#ifdef DEBUG_PRINTF_APP
//printf("[App] Volume increased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, volString, 7);
#endif
			}

			runPlayer(&ev, &appContext); /* Run player in background */
			/*ACA pongo lo que se hace para hacer la fft
			 * despues me imagino irá en un PIT
			 * para actualizar los valores cada 250ms capaz//TODO
			 * */
//				adaptFFT(src, dst, 1024);
//				fft(inputF32, outputF32, 1);
//				fftGetMag(inputF32, outputF32);
//				fftMakeBines8(src, dst);
			runMenu(&ev, &appContext); /* Run menu in background */
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
#ifdef DEBUG_PRINTF_APP
	BOARD_InitDebugConsole();
#else
	/* uart */
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 9600;
	config.enableTx = true;
	config.enableRx = true;
	UART_Init(UART0, &config, CLOCK_GetFreq(UART0_CLK_SRC));
	UART_EnableInterrupts(UART0,
			kUART_RxDataRegFullInterruptEnable
					| kUART_RxOverrunInterruptEnable);
	EnableIRQ(UART0_RX_TX_IRQn);
#endif

#ifdef APP_KINETIS_LEDS
	/* Init kinetis ON/OFF leds */
	LED_RED_INIT(LOGIC_LED_ON);
	LED_GREEN_INIT(LOGIC_LED_OFF);
#endif

	/* Init sub-drivers */
	pit_config_t pit_config;
	PIT_GetDefaultConfig(&pit_config);
	PIT_Init(PIT, &pit_config);

	/* Init Botonera */
	BUTTON_Init();

	/* Init Helix MP3 Decoder */
	MP3DecoderInit();

	/* Init DAC */
	DAC_Wrapper_Init();		//Init DMAMUX, EDMA, PDB, DAC
	DAC_Wrapper_Loop(false);
	DAC_Wrapper_Start_Trigger();
	DAC_Wrapper_Sleep();

	/* Init SD wrapper */
	SDWRAPPER_Init(cbackin, cbackout); //Init PIT, SD
	SDWRAPPER_SetInterruptEnable(false);
	/* Led matrix */
	LEDMATRIX_Init(); //PIT, DMA, DMAMUX, FTM
	/* Init event handlers */
	EVHANDLER_InitHandlers();

	/* Init FFT */
	fftInit(CFFT_1024);

	/* Reset app context */
	resetAppContext();
	/*encoder */
	EncoderRegister();

	return true; //TODO agregar verificaciones al resto de los inits

}

void switchAppState(app_state_t current, app_state_t target) {

	switch (target) {
	case kAPP_STATE_OFF: /* Can come from IDDLE or PLAYING */
#ifndef DEBUG_PRINTF_APP
		UART_WriteBlocking(UART0, "10A\r\n", 6);
#endif
		if (current == kAPP_STATE_IDDLE) {
			prepareForSwitchOff();
			appContext.appState = target;
		} else if (current == kAPP_STATE_PLAYING) {
			DAC_Wrapper_Clear_Data_Array();
			DAC_Wrapper_Clear_Next_Buffer();
			DAC_Wrapper_Sleep();
			appContext.playerContext.firstDacTransmition = true;
			prepareForSwitchOff();
			appContext.appState = target;

		}
		break;
	case kAPP_STATE_IDDLE: /* Can come from OFF or PLAYING */
		if (current == kAPP_STATE_OFF) {
#ifndef DEBUG_PRINTF_APP
			UART_WriteBlocking(UART0, "10P\r\n", 6);
#endif
			prepareForSwitchOn();
			appContext.appState = target;
		} else if (current == kAPP_STATE_PLAYING) {
			//TODO: Stop player, spectrogram..
			DAC_Wrapper_Sleep();
			appContext.playerContext.firstDacTransmition = true;
			appContext.appState = target;
		}
		break;
	case kAPP_STATE_PLAYING:
		if ((current == kAPP_STATE_IDDLE) || (current == kAPP_STATE_PLAYING)) {
			//TODO: Spectrogram...

			if (!appContext.playerContext.songResumed) {

				resetPlayerContext();

				char *songName = FSEXP_getMP3Path();
				MP3LoadFile(&songName[1]);

#ifdef DEBUG_PRINTF_APP
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
#else
				UART_WriteBlocking(UART0, "08P\r\n", 6);

				if (MP3GetTagData(&appContext.playerContext.ID3Data)) {
					SDMMC_OSADelay(100U);
					uint8_t i = 0;
					while (appContext.playerContext.ID3Data.title[i] != '\0') {
						i++;
					}
					appContext.playerContext.ID3Data.title[i++] = '\r';
					appContext.playerContext.ID3Data.title[i++] = '\n';
					appContext.playerContext.ID3Data.title[i++] = '\0';
					id3Buffer[1] = '1';
					copyFname(&(id3Buffer[2]),
							appContext.playerContext.ID3Data.title);
					UART_WriteBlocking(UART0, id3Buffer, i + 2);

					SDMMC_OSADelay(100U);

					i = 0;

					while (appContext.playerContext.ID3Data.artist[i] != '\0') {
						i++;
					}
					appContext.playerContext.ID3Data.artist[i++] = '\r';
					appContext.playerContext.ID3Data.artist[i++] = '\n';
					appContext.playerContext.ID3Data.artist[i++] = '\0';
					id3Buffer[1] = '2';
					copyFname(&(id3Buffer[2]),
							appContext.playerContext.ID3Data.artist);
					UART_WriteBlocking(UART0, id3Buffer, i + 2);

					SDMMC_OSADelay(100U);

					i = 0;

					while (appContext.playerContext.ID3Data.album[i] != '\0') {
						i++;
					}
					appContext.playerContext.ID3Data.album[i++] = '\r';
					appContext.playerContext.ID3Data.album[i++] = '\n';
					appContext.playerContext.ID3Data.album[i++] = '\0';
					id3Buffer[1] = '3';
					copyFname(&(id3Buffer[2]),
							appContext.playerContext.ID3Data.album);
					UART_WriteBlocking(UART0, id3Buffer, i + 2);

					SDMMC_OSADelay(100U);

					i = 0;

					while (appContext.playerContext.ID3Data.trackNum[i] != '\0') {
						i++;
					}
					appContext.playerContext.ID3Data.trackNum[i++] = '\r';
					appContext.playerContext.ID3Data.trackNum[i++] = '\n';
					appContext.playerContext.ID3Data.trackNum[i++] = '\0';
					id3Buffer[1] = '4';
					copyFname(&(id3Buffer[2]),
							appContext.playerContext.ID3Data.trackNum);
					UART_WriteBlocking(UART0, id3Buffer, i + 2);

					SDMMC_OSADelay(100U);

					i = 0;

					while (appContext.playerContext.ID3Data.year[i] != '\0') {
						i++;
					}
					appContext.playerContext.ID3Data.year[i++] = '\r';
					appContext.playerContext.ID3Data.year[i++] = '\n';
					appContext.playerContext.ID3Data.year[i++] = '\0';
					id3Buffer[1] = '5';
					copyFname(&(id3Buffer[2]),
							appContext.playerContext.ID3Data.year);
					UART_WriteBlocking(UART0, id3Buffer, i + 2);

					SDMMC_OSADelay(100U);
				}
#endif

				//Empiezo por el buffer 1
				appContext.playerContext.res = MP3GetDecodedFrame(
						(int16_t*) getbuffer1(),
						MP3_DECODED_BUFFER_SIZE,
						&appContext.playerContext.sampleCount, 0);

				MP3_Adapt_Signal((int16_t*) getbuffer1(), getbuffer1(),
						appContext.playerContext.sampleCount,
						appContext.volume);
				MP3_Set_Sample_Rate(appContext.playerContext.sr_,
						appContext.playerContext.ch_);

			} else {
				appContext.playerContext.songResumed = false;
				appContext.playerContext.songPaused = false;
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
	FSM_menu(events, context);
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
	appContext.volume = 15;
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
#else
	UART_WriteBlocking(UART0, "07I\r\n", 6);
#endif

}

void cbackout(void) {
#ifdef DEBUG_PRINTF_APP
	printf("[App] SD Card removed.\r\n");
#else
//	DAC_Wrapper_Clear_Data_Array();
//	DAC_Wrapper_Clear_Next_Buffer();
	UART_WriteBlocking(UART0, "07O\r\n", 6);
#endif
}
void adaptFFT(float32_t *src, float32_t *dst, uint16_t cnt) {
	uint16_t i = 0;
	for (i = 0; i < cnt; i++) {
		dst[i * 2] = src[i];
	}
}
