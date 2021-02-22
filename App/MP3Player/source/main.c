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
#include "fsl_rtc.h"
#include "../HAL/SD_Detect_Wraper.h"
#include "../HAL/DAC_Wrapper.h"
#include "FS_explorer.h"

#include <fft.h>

#include "../HAL/button.h"
#include "encoder.h"
#include "FSM.h"
#include "general.h"
#include "../HAL/LED_Matrix.h"

#include "equalizer.h"

/**********************************************************************************************
 *                                          DEFINES                                           *
 **********************************************************************************************/

/**********************************************************************************************
 *                                    TYPEDEFS AND ENUMS                                      *
 **********************************************************************************************/
typedef enum {ANIM_OFF, ANIM_YELLOW, ANIM_CYAN, ANIM_VIOLET, ANIM_BLUE, ANIM_GREEN}anim_state;
/**********************************************************************************************
 *                        FUNCTION DECLARATION WITH LOCAL SCOPE                               *
 **********************************************************************************************/

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
void runAnimation(void);
void cbackout(void);
void sendInitialDate(void);
void initAnimation(void);
/**********************************************************************************************
 *                              VARIABLES WITH LOCAL SCOPE                                    *
 **********************************************************************************************/

static app_context_t appContext;
static char volString[7];
static char id3Buffer[ID3_MAX_FIELD_SIZE + 2];
static rtc_datetime_t date;
static uint8_t animState;
static uint8_t aniMatrix[8][8*8];
/**********************************************************************************************
 *                                         MAIN                                               *
 **********************************************************************************************/

int main(void) {

	initDevice(); /* Init device */

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
		    RTC_GetDatetime(RTC, &date);
			sendInitialDate();
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
			} else if (ev.btn_evs.pause_play_button && appContext.playerContext.songActive) {
				appContext.playerContext.songResumed = true;
				switchAppState(appContext.appState, kAPP_STATE_PLAYING);
#ifdef DEBUG_PRINTF_APP
				printf("[App] Resumed playing.\n");
				#else
				UART_WriteBlocking(UART0, (uint8_t*) "08P\r\n", 6);
#endif
			} else if (ev.encoder_evs.back_button && appContext.volume > 0) {
				appContext.volume--;

#ifdef DEBUG_PRINTF_APP
printf("[App] Volume decreased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, (uint8_t*) volString, 7);
#endif
			} else if (ev.encoder_evs.next_button && appContext.volume < 30) {
				appContext.volume++;
#ifdef DEBUG_PRINTF_APP
printf("[App] Volume increased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, (uint8_t*) volString, 7);
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
				UART_WriteBlocking(UART0, (uint8_t*)"08S\r\n", 6);
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
					UART_WriteBlocking(UART0, (uint8_t*)myPackPath, i);

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
				UART_WriteBlocking(UART0, (uint8_t*)volString, 7);
#endif
			} else if (ev.encoder_evs.next_button && appContext.volume < 30) {
				appContext.volume++;
#ifdef DEBUG_PRINTF_APP
//printf("[App] Volume increased, set to: %d\n", appContext.volume);
#else
				volString[2] = (appContext.volume / 10) + '0';
				volString[3] = (appContext.volume
						- (appContext.volume / 10) * 10) + '0';
				UART_WriteBlocking(UART0, (uint8_t*)volString, 7);
#endif
			}

			runPlayer(&ev, &appContext); /* Run player in background */
			/*ACA pongo lo que se hace para hacer la fft
			 * despues me imagino irá en un PIT
			 * para actualizar los valores cada 250ms capaz//TODO
			 * */


			runMenu(&ev, &appContext); /* Run menu in background */
			break;

		default:
			switchAppState(appContext.appState, kAPP_STATE_OFF);
			printf("error");
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

	/*Init equalizer*/
	init_equalizer();
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
	LEDMATRIX_SetCb(runAnimation);
	LEDMATRIX_EnableAnimation();
	LEDMATRIX_Enable();
	LEDMATRIX_Pause();
	initAnimation();
	/* Init event handlers */
	EVHANDLER_InitHandlers();

	/* Init FFT */
	fftInit(CFFT_256);

	/* Reset app context */
	resetAppContext();
	/*encoder */
	EncoderRegister();
	/* FSM init */
	FSM_init();
	/*   */
	/* rtc init */
	rtc_config_t rtcConfig;
    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);
    RTC_SetClockSource(RTC);
    /* Set a start date time and start RTC */
       date.year   = 2021U;
       date.month  = 2U;
       date.day    = 22U;
       date.hour   = 11U;
       date.minute = 00U;
       date.second = 00U;
       /* RTC time counter has to be stopped before setting the date & time in the TSR register */
       RTC_StopTimer(RTC);
       RTC_SetDatetime(RTC, &date);
       /* Enable at the NVIC */
       EnableIRQ(RTC_IRQn);
       /* Start the RTC time counter */
       RTC_StartTimer(RTC);
       sendInitialDate();

#ifndef DEBUG_PRINTF_APP
		UART_WriteBlocking(UART0, (uint8_t*) "10A\r\n", 6);
#endif

	return true; //TODO agregar verificaciones al resto de los inits

}

void switchAppState(app_state_t current, app_state_t target) {

	LEDMATRIX_CleanDisplay();
	switch (target) {
	case kAPP_STATE_OFF: /* Can come from IDDLE or PLAYING */
#ifndef DEBUG_PRINTF_APP
		UART_WriteBlocking(UART0, (uint8_t*) "10A\r\n", 6);
#endif
		if (current == kAPP_STATE_IDDLE) {
			prepareForSwitchOff();
			appContext.appState = target;
		} else if (current == kAPP_STATE_PLAYING) {
			DAC_Wrapper_Clear_Data_Array();
			DAC_Wrapper_Clear_Next_Buffer();
			DAC_Wrapper_Sleep();
			resetAppContext();
			prepareForSwitchOff();
			appContext.appState = target;

		}
		break;
	case kAPP_STATE_IDDLE: /* Can come from OFF or PLAYING */
		if (current == kAPP_STATE_OFF) {
			LEDMATRIX_EnableAnimation();
#ifndef DEBUG_PRINTF_APP
			UART_WriteBlocking(UART0, (uint8_t*) "10P\r\n", 6);
#endif
			prepareForSwitchOn();
			appContext.appState = target;
		} else if (current == kAPP_STATE_PLAYING) {
			LEDMATRIX_EnableAnimation();
			DAC_Wrapper_Sleep();
			appContext.playerContext.firstDacTransmition = true;
			appContext.appState = target;
		}
		break;
	case kAPP_STATE_PLAYING:
		if ((current == kAPP_STATE_IDDLE) || (current == kAPP_STATE_PLAYING)) {
			LEDMATRIX_DisableAnimation();
			LEDMATRIX_CleanDisplay();
			if (!appContext.playerContext.songResumed) {

				resetPlayerContext();

				appContext.playerContext.songActive = true;

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
				UART_WriteBlocking(UART0, (uint8_t*) "08P\r\n", 6);

				if (MP3GetTagData(&appContext.playerContext.ID3Data)) {
				    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
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
					UART_WriteBlocking(UART0,(uint8_t*)  id3Buffer, i + 2);

				    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

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
					UART_WriteBlocking(UART0,(uint8_t*)  id3Buffer, i + 2);

				    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

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
					UART_WriteBlocking(UART0, (uint8_t*) id3Buffer, i + 2);

				    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

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
					UART_WriteBlocking(UART0, (uint8_t*) id3Buffer, i + 2);

				    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

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
					UART_WriteBlocking(UART0,(uint8_t*)  id3Buffer, i + 2);

				    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
				}
#endif

				//Empiezo por el buffer 1
				appContext.playerContext.res = MP3GetDecodedFrame(
						(int16_t*) getbuffer1(),
						MP3_DECODED_BUFFER_SIZE,
						&appContext.playerContext.sampleCount);

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

void initAnimation(void){

	//Vacio
	/*
	 *
	 */

	//Squiggly_1
	aniMatrix[1][7+8*6] = ANIM_GREEN;
	aniMatrix[1][6+8*6] = ANIM_GREEN;
	aniMatrix[1][5+8*6] = ANIM_GREEN;
	aniMatrix[1][4+8*6] = ANIM_GREEN;
	aniMatrix[2][3+8*6] = ANIM_GREEN;
	aniMatrix[3][3+8*6] = ANIM_GREEN;
	aniMatrix[4][2+8*6] = ANIM_GREEN;
	aniMatrix[5][2+8*6] = ANIM_GREEN;
	aniMatrix[6][1+8*6] = ANIM_GREEN;
	aniMatrix[6][0+8*6] = ANIM_GREEN;

	//Squiggly_2
	aniMatrix[6][7+8*5] = ANIM_GREEN;
	aniMatrix[5][6+8*5] = ANIM_GREEN;
	aniMatrix[4][6+8*5] = ANIM_GREEN;
	aniMatrix[3][5+8*5] = ANIM_GREEN;
	aniMatrix[2][5+8*5] = ANIM_GREEN;
	aniMatrix[1][4+8*5] = ANIM_GREEN;
	aniMatrix[1][3+8*5] = ANIM_GREEN;
	aniMatrix[1][2+8*5] = ANIM_GREEN;
	aniMatrix[2][1+8*5] = ANIM_GREEN;
	aniMatrix[3][0+8*5] = ANIM_GREEN;

	//Squiggly_3
	aniMatrix[4][7+8*4] = ANIM_GREEN;
	aniMatrix[4][6+8*4] = ANIM_GREEN;
	aniMatrix[3][5+8*4] = ANIM_GREEN;
	aniMatrix[2][4+8*4] = ANIM_GREEN;
	aniMatrix[1][3+8*4] = ANIM_GREEN;
	aniMatrix[1][2+8*4] = ANIM_GREEN;
	aniMatrix[1][1+8*4] = ANIM_GREEN;
	aniMatrix[1][0+8*4] = ANIM_GREEN;

	//M
	aniMatrix[1][6+8*3] = ANIM_YELLOW;
	aniMatrix[2][6+8*3] = ANIM_YELLOW;
	aniMatrix[3][6+8*3] = ANIM_YELLOW;
	aniMatrix[4][6+8*3] = ANIM_YELLOW;
	aniMatrix[5][6+8*3] = ANIM_YELLOW;
	aniMatrix[6][6+8*3] = ANIM_YELLOW;
	aniMatrix[5][5+8*3] = ANIM_YELLOW;
	aniMatrix[4][4+8*3] = ANIM_YELLOW;
	aniMatrix[4][3+8*3] = ANIM_YELLOW;
	aniMatrix[5][2+8*3] = ANIM_YELLOW;
	aniMatrix[1][1+8*3] = ANIM_YELLOW;
	aniMatrix[2][1+8*3] = ANIM_YELLOW;
	aniMatrix[3][1+8*3] = ANIM_YELLOW;
	aniMatrix[4][1+8*3] = ANIM_YELLOW;
	aniMatrix[5][1+8*3] = ANIM_YELLOW;
	aniMatrix[6][1+8*3] = ANIM_YELLOW;

	//A
	aniMatrix[1][1+8*2] = ANIM_CYAN;
	aniMatrix[2][1+8*2] = ANIM_CYAN;
	aniMatrix[3][1+8*2] = ANIM_CYAN;
	aniMatrix[4][1+8*2] = ANIM_CYAN;
	aniMatrix[1][6+8*2] = ANIM_CYAN;
	aniMatrix[2][6+8*2] = ANIM_CYAN;
	aniMatrix[3][6+8*2] = ANIM_CYAN;
	aniMatrix[4][6+8*2] = ANIM_CYAN;
	aniMatrix[3][5+8*2] = ANIM_CYAN;
	aniMatrix[3][4+8*2] = ANIM_CYAN;
	aniMatrix[3][3+8*2] = ANIM_CYAN;
	aniMatrix[3][2+8*2] = ANIM_CYAN;
	aniMatrix[5][5+8*2] = ANIM_CYAN;
	aniMatrix[6][4+8*2] = ANIM_CYAN;
	aniMatrix[6][3+8*2] = ANIM_CYAN;
	aniMatrix[5][2+8*2] = ANIM_CYAN;

	//G
	aniMatrix[1][5+8*1] = ANIM_VIOLET;
	aniMatrix[1][4+8*1] = ANIM_VIOLET;
	aniMatrix[1][3+8*1] = ANIM_VIOLET;
	aniMatrix[1][2+8*1] = ANIM_VIOLET;
	aniMatrix[1][1+8*1] = ANIM_VIOLET;
	aniMatrix[6][6+8*1] = ANIM_VIOLET;
	aniMatrix[5][6+8*1] = ANIM_VIOLET;
	aniMatrix[4][6+8*1] = ANIM_VIOLET;
	aniMatrix[3][6+8*1] = ANIM_VIOLET;
	aniMatrix[2][6+8*1] = ANIM_VIOLET;
	aniMatrix[1][6+8*1] = ANIM_VIOLET;
	aniMatrix[6][5+8*1] = ANIM_VIOLET;
	aniMatrix[6][4+8*1] = ANIM_VIOLET;
	aniMatrix[6][3+8*1] = ANIM_VIOLET;
	aniMatrix[6][2+8*1] = ANIM_VIOLET;
	aniMatrix[6][1+8*1] = ANIM_VIOLET;
	aniMatrix[2][1+8*1] = ANIM_VIOLET;
	aniMatrix[3][1+8*1] = ANIM_VIOLET;
	aniMatrix[3][2+8*1] = ANIM_VIOLET;
	aniMatrix[3][3+8*1] = ANIM_VIOLET;
	aniMatrix[3][4+8*1] = ANIM_VIOLET;

	//T
	aniMatrix[6][6+8*0] = ANIM_BLUE;
	aniMatrix[6][5+8*0] = ANIM_BLUE;
	aniMatrix[6][4+8*0] = ANIM_BLUE;
	aniMatrix[6][3+8*0] = ANIM_BLUE;
	aniMatrix[6][2+8*0] = ANIM_BLUE;
	aniMatrix[6][1+8*0] = ANIM_BLUE;
	aniMatrix[5][4+8*0] = ANIM_BLUE;
	aniMatrix[4][4+8*0] = ANIM_BLUE;
	aniMatrix[3][4+8*0] = ANIM_BLUE;
	aniMatrix[2][4+8*0] = ANIM_BLUE;
	aniMatrix[1][4+8*0] = ANIM_BLUE;
	aniMatrix[5][3+8*0] = ANIM_BLUE;
	aniMatrix[4][3+8*0] = ANIM_BLUE;
	aniMatrix[3][3+8*0] = ANIM_BLUE;
	aniMatrix[2][3+8*0] = ANIM_BLUE;
	aniMatrix[1][3+8*0] = ANIM_BLUE;

}

void runAnimation(void){
	uint16_t hor, vert;
	if(animState == 0){
		animState = 63;
	}
	else{
		animState--;
	}
	for(hor = 0; hor < 8; hor++){
		for(vert = 0; vert < 8; vert++){
			switch(aniMatrix[vert][hor+animState]){
			case ANIM_OFF:
				LEDMATRIX_SetLed(vert, hor, 0, 0, 0);
				break;
			case ANIM_YELLOW:
				LEDMATRIX_SetLed(vert, hor, 1, 1, 0);
				break;
			case ANIM_CYAN:
				LEDMATRIX_SetLed(vert, hor, 0, 1, 1);
				break;
			case ANIM_VIOLET:
				LEDMATRIX_SetLed(vert, hor, 1, 0, 1);
				break;
			case ANIM_BLUE:
				LEDMATRIX_SetLed(vert, hor, 0, 0, 5);
				break;
			case ANIM_GREEN:
				LEDMATRIX_SetLed(vert, hor, 0, 5, 0);
				break;
			default: break;
			}
		}
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
	appContext.playerContext.songActive = false;
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
	UART_WriteBlocking(UART0, (uint8_t*)"07I\r\n", 6);
#endif

}

void cbackout(void) {
#ifdef DEBUG_PRINTF_APP
	printf("[App] SD Card removed.\r\n");
#else
//	DAC_Wrapper_Clear_Data_Array();
//	DAC_Wrapper_Clear_Next_Buffer();
	UART_WriteBlocking(UART0, (uint8_t*)"07O\r\n", 6);
#endif

	resetAppContext();
}


void sendInitialDate(void){
    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    char time_string[20];
	time_string[0]='1';
	time_string[1]='2';
	time_string[6]='/';
	time_string[9]='/';
	time_string[14]=':';
	time_string[17]='\r';
	time_string[18]='\n';
	time_string[19]='\0';
	uint16_t aux_m, aux_c, aux_d, aux_u;
	aux_m= (date.year / 1000);
	time_string[2] = aux_m+'0';
	aux_c= (date.year-aux_m*1000)/100;
	time_string[3] = aux_c+'0';
	aux_d=(date.year-aux_m*1000-aux_c*100)/10;
	time_string[4] = aux_d+'0';
	aux_u=date.year-aux_m*1000-aux_c*100-aux_d*10;
	time_string[5] = aux_u+'0';//Year
	aux_d=date.month/10;
	time_string[7] = aux_d+'0';//Month
	aux_u=date.month-aux_d*10;
	time_string[8] = aux_u+'0';
	aux_d=date.day/10;
	time_string[10] = aux_d+'0';//Day
	aux_u=date.day-aux_d*10;
	time_string[11] = aux_u+'0';
	aux_d=date.hour/10;
	time_string[12] = aux_d+'0';//Hour
	aux_u=date.hour-aux_d*10;
	time_string[13] = aux_u+'0';
	aux_d=date.minute/10;
	time_string[15] = aux_d+'0';//Minute
	aux_u=date.minute-aux_d*10;
	time_string[16] = aux_u+'0';
#ifndef DEBUG_PRINTF_APP
	UART_WriteBlocking(UART0, (uint8_t*) time_string, 20);
#endif
    SDK_DelayAtLeastUs(100U * 1000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);


}
