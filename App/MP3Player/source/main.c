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

/**********************************************************************************************
 *                                          DEFINES                                           *
 **********************************************************************************************/

#define MAX_DAC				4095
#define MP3_MAX_VALUE		100000//32767
#define MP3_MIN_VALUE		-100000//-32768
#define MP3_GAP				MP3_MAX_VALUE - MP3_MIN_VALUE

/**********************************************************************************************
 *                                    TYPEDEFS AND ENUMS                                      *
 **********************************************************************************************/

typedef enum {
	kAPP_STATE_OFF,
	kAPP_STATE_IDDLE,
	kAPP_STATE_PLAYING
} app_state_t;

typedef enum {
	kAPP_MENU_MAIN,
	kAPP_MENU_FILESYSTEM,
	kAPP_MENU_VOLUME,
	kAPP_MENU_EQUALIZER,
	kAPP_MENU_SPECTROGRAM
} menu_state_t;

typedef struct{
	app_state_t appState;
	menu_state_t menuState;
	bool spectrogramEnable;
	uint8_t volume;
	char* currentFile;
}app_context_t;

/**********************************************************************************************
 *                        FUNCTION DECLARATION WITH LOCAL SCOPE                               *
 **********************************************************************************************/

void runMenu(event_t* events, app_context_t* context);
void runPlayer(event_t* events, app_context_t* context);
void switchAppState(app_state_t current, app_state_t target);
void resetAppContext(void);
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

static short buffer_1[MP3_DECODED_BUFFER_SIZE];
static short buffer_2[MP3_DECODED_BUFFER_SIZE];

mp3_decoder_frame_data_t frameData;
mp3_decoder_tag_data_t ID3Data;

/**********************************************************************************************
 *                                         MAIN                                               *
 **********************************************************************************************/

int main(void) {

	initDevice();																		/* Init device */
	prepareForSwitchOff();																/*and turn off */

	while(true){
		event_t ev;

		EVHANDLER_GetEvents(&ev);														/* Get events */

		switch(appContext.appState){

				/***************/
				/*  OFF STATE  */
				/***************/

			case kAPP_STATE_OFF:
				switchOffKinetis();														/* Turn off */
				switchAppState(appContext.appState, kAPP_STATE_IDDLE);					/* Go back to IDDLE */

				break;

				/***************/
				/* IDDLE STATE */
				/***************/

			case kAPP_STATE_IDDLE:
				if(SDWRAPPER_GetMounted()&&SDWRAPPER_getJustIn()){
					appContext.currentFile = FSEXP_exploreFS(FSEXP_ROOT_DIR);			/* Explore filesystem */
					#ifdef DEBUG_PRINTF_APP
					appContext.currentFile = FSEXP_getNext();
					printf("[App] Pointing currently to: %s\n", appContext.currentFile);
					#endif
				}
				if(SDWRAPPER_getJustOut()){												/* If SD is removed */
					if(appContext.menuState == kAPP_MENU_FILESYSTEM){					/* and menu is exploring FS*/
						appContext.menuState = kAPP_MENU_MAIN;							/* Go back to main menu */
					}
				}
				if(ev.btn_evs.off_on_button){
					switchAppState(appContext.appState, kAPP_STATE_OFF);
				}
				if(SDWRAPPER_GetMounted()&&SDWRAPPER_getSDInserted()){
					if(ev.btn_evs.next_button){
						if(FSEXP_getNext()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.btn_evs.prev_button){
						if(FSEXP_getPrev()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.btn_evs.enter_button){
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Opened %s\n", appContext.currentFile);
						#endif
						if(FSEXP_openSelected()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.btn_evs.back_button){

						if(FSEXP_goBackDir()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Went back a directory\n");
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.fsexp_evs.play_music){
						switchAppState(appContext.appState, kAPP_STATE_PLAYING);
					}
				}
				runMenu(&ev, &appContext);										/* Run menu in background */
				break;

				/***************/
				/*PLAYING STATE*/
				/***************/

			case kAPP_STATE_PLAYING:

				if(SDWRAPPER_getJustOut()){											/* If SD is removed */
					if(appContext.menuState == kAPP_MENU_FILESYSTEM){				/* and menu is exploring FS*/
						appContext.menuState = kAPP_MENU_MAIN;						/* Go back to main menu */
						//TODO stop music, stop spectogram
						//...
						switchAppState(appContext.appState, kAPP_STATE_IDDLE); 		/* Return to iddle state */
						break;
					}
				}
				if(ev.btn_evs.off_on_button){
					switchAppState(appContext.appState, kAPP_STATE_OFF);
				}
				if(SDWRAPPER_GetMounted()&&SDWRAPPER_getSDInserted()){
					if(ev.btn_evs.next_button){
						if(FSEXP_getNext()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.btn_evs.prev_button){
						if(FSEXP_getPrev()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.btn_evs.enter_button){
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Opened %s\n", appContext.currentFile);
						#endif
						if(FSEXP_openSelected()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.btn_evs.back_button){

						if(FSEXP_goBackDir()){
							appContext.currentFile = FSEXP_getFilename();
						}
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Went back a directory\n");
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
					}
					if(ev.fsexp_evs.play_music){
						#ifdef DEBUG_PRINTF_APP
						printf("[App] Pointing currently to: %s\n", appContext.currentFile);
						#endif
						//TODO: Reiniciar todo para reproducir la proxima cancion.
					}
				}

				runMenu(&ev, &appContext);											/* Run menu in background */
				runPlayer(&ev, &appContext);										/* Run player in background */
				break;


			default: break;
		}
	}
	return 0;
}

/**********************************************************************************************
 *                        FUNCTION DEFINITIONS WITH LOCAL SCOPE                               *
 **********************************************************************************************/

int initDevice(void){

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

	/* Init wakeup interruption */
	SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
	NVIC_EnableIRQ(APP_WAKEUP_BUTTON_IRQ);	//NVIC del wakeup (SW2)
	PORT_SetPinInterruptConfig(APP_WAKEUP_BUTTON_PORT, APP_WAKEUP_BUTTON_GPIO_PIN, APP_WAKEUP_BUTTON_IRQ_TYPE); //PCR del wakeup (SW2)

	/* Botonera */
	NVIC_EnableIRQ(PORTD_IRQn);	//NVIC botonera PORTD
	NVIC_EnableIRQ(BOARD_SW3_IRQ);	//NVIC SW3 PORTA
	PORT_SetPinInterruptConfig(PORTD, 0U, kPORT_InterruptRisingEdge); //PCR PORTD0
	PORT_SetPinInterruptConfig(PORTD, 1U, kPORT_InterruptRisingEdge); //PCR PORTD1
	PORT_SetPinInterruptConfig(PORTD, 2U, kPORT_InterruptRisingEdge); //PCR PORTD2
	PORT_SetPinInterruptConfig(PORTD, 3U, kPORT_InterruptRisingEdge); //PCR PORTD3
	PORT_SetPinInterruptConfig(BOARD_SW3_PORT, BOARD_SW3_GPIO_PIN, kPORT_InterruptFallingEdge); //PCR SW3

	/* Init Helix MP3 Decoder */
	MP3DecoderInit();

	/* Init SD wrapper */
	SDWRAPPER_Init(cbackin, cbackout); //Init PIT, SD
	SDWRAPPER_SetInterruptEnable(false);

	/* Init DAC */
	DAC_Wrapper_Init();		//Init DMAMUX, EDMA, PDB, DAC
	DAC_Wrapper_Loop(false);

	/* Init event handlers */
	EVHANDLER_InitHandlers();

	/* Reset app context */
	resetAppContext();

	return true; //TODO agregar verificaciones al resto de los inits

}

void switchAppState(app_state_t current, app_state_t target){
	switch(target){
	case kAPP_STATE_OFF: 							/* Can come from IDDLE or PLAYING */
		if(current == kAPP_STATE_IDDLE){
			prepareForSwitchOff();
			appContext.appState = target;
		}
		else if(current == kAPP_STATE_PLAYING){
			prepareForSwitchOff();
			appContext.appState = target;
		}
		break;
	case kAPP_STATE_IDDLE:							/* Can come from OFF or PLAYING */
		if(current == kAPP_STATE_OFF){
			prepareForSwitchOn();
			appContext.appState = target;
		}
		else if(current == kAPP_STATE_PLAYING){
			//TODO: Stop player, spectrogram..
			appContext.appState = target;
		}
		break;
	case kAPP_STATE_PLAYING:						/* Can only come from IDDLE */
		if(current == kAPP_STATE_IDDLE){
			//TODO: Start player, spectrogram..
			#ifdef DEBUG_PRINTF_APP
			printf("[App] Playing music..\n");
			#endif
			appContext.appState = target;
		}
		break;

	default: break;
	}
}

void runMenu(event_t* events, app_context_t* context){
	//TODO: La fsm del menu
}
void runPlayer(event_t* events, app_context_t* context){
	//TODO: Implementar aca el reproductor y el espectrograma (ver si no van a funcar a interrupciones tho)
}

char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void prepareForSwitchOff(void){
	SDWRAPPER_SetCardPower(false);
	BOARD_SetPortsForWakeupSW2(true);
}

void prepareForSwitchOn(void){
	BOARD_SetPortsForWakeupSW2(false);
	SDWRAPPER_SetCardPower(true);
	SDWRAPPER_SetInsertedFlag();
	SDWRAPPER_SetInterruptEnable(true);
}

void resetAppContext(void){
	appContext.appState = kAPP_STATE_OFF;
	appContext.menuState = kAPP_MENU_MAIN;
	appContext.spectrogramEnable = false;
	appContext.volume = 50;
	appContext.currentFile = NULL;
}

void switchOffKinetis(void){
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
	SMC_PostExitStopModes();						//Despues de apretar el SW2, se sigue en esta linea de codigo

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
/*
#ifdef DEBUG_PRINTF_INFO
	uint32_t sr = 0;
#endif
	uint16_t sampleCount;
if (getJustIn()) {
						if (MP3LoadFile("test.mp3", "test.wav")) {
							int i = 0;
							if (MP3GetTagData(&ID3Data)) {
#ifdef DEBUG_PRINTF_APP
								printf("\nSONG INFO\n");
								printf("TITLE: %s\n", ID3Data.title);
								printf("ARTIST: %s\n", ID3Data.artist);
								printf("ALBUM: %s\n", ID3Data.album);
								printf("TRACK NUM: %s\n", ID3Data.trackNum);
								printf("YEAR: %s\n", ID3Data.year);
#endif
							}
							DAC_Wrapper_Start_Trigger();

							//Empiezo por el buffer 1
							mp3_decoder_result_t res = MP3GetDecodedFrame(buffer_1,
							MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);
							bool using_buffer_1 = true;

							uint16_t sr_ = kMP3_44100Hz;	//Default config 4 mp3 stereo
							uint8_t ch_ = kMP3_Stereo;
							MP3_Set_Sample_Rate(sr_, ch_);
							//Por defecto ya está configurado así, solo lo explicito y ayuda
							//si hay que cambiarlo mientras corre el codigo

							while (true) {

#ifdef DEBUG_PRINTF_INFO
								printf("\n[APP] Frame %d decoding started.\n", i);
#endif

								if (res == MP3DECODER_NO_ERROR) {

									if (DAC_Wrapper_Is_Transfer_Done() || i == 0) {	//Entro en la primera o cuando ya transmiti

										MP3GetLastFrameData(&frameData);

										//No debería cambiar el sample rate entre frame y frame
										//Pero si lo hace...
										if ((sr_ != frameData.sampleRate)
												|| (ch_ != frameData.channelCount)) {
											sr_ = frameData.sampleRate;
											ch_ = frameData.channelCount;
											MP3_Set_Sample_Rate(sr_, ch_);
										}

										DAC_Wrapper_Clear_Transfer_Done();

										//uint16_t* nullptr = NULL;
										if (using_buffer_1) {
											//Envio el buffer 1 al dac
											DAC_Wrapper_Set_Data_Array(&buffer_1,
													frameData.sampleCount);
											DAC_Wrapper_Set_Next_Buffer(&buffer_2);

											//Cargo el y normalizo el buffer 2
											res = MP3GetDecodedFrame(buffer_2,
											MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);
											uint16_t j;
											for (j = 0; j < frameData.sampleCount; j++) {
												buffer_2[j] = (uint16_t) ((buffer_2[j]
														+ 100000) * 4095 / 200000);
											}
										} else {
											//Envio el buffer 2 al dac
											DAC_Wrapper_Set_Data_Array(&buffer_2,
													frameData.sampleCount);
											DAC_Wrapper_Set_Next_Buffer(&buffer_1);

											//Cargo el y normalizo el buffer 1
											res = MP3GetDecodedFrame(buffer_1,
											MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);
											uint16_t j;
											for (j = 0; j < frameData.sampleCount; j++) {
												buffer_1[j] = (uint16_t) ((buffer_1[j]
														+ 100000) * 4095 / 200000);
											}
										}

										using_buffer_1 = !using_buffer_1;//Cambio el buffer al siguiente

										i++;
									}

			#ifdef DEBUG_PRINTF_INFO
									printf("[APP] Wrote %d bytes to wav.\n", wrote);

									printf("[APP] Frame %d decoded.\n", i);
			#endif

			#ifdef DEBUG_PRINTF_INFO
									sr = frameData.sampleRate;

									printf("[APP] FRAME SAMPLE RATE: %d \n", sr);
			#endif

			#ifdef DEBUG_FRAME_DELAY
									printf("[APP] Un pequenio delay para el dios de la SD bro.\n");
									while(!nextFrameFlag){
										//wait
									}
									nextFrameFlag = false;
			#endif

								} else if (res == MP3DECODER_FILE_END) {
			#ifdef DEBUG_PRINTF_APP
										printf("[APP] FILE ENDED. Decoded %d frames.\n", i - 1);
			#endif
									finished = true;
									break;
								}
							}
						}
					}
			close_file_wav();
				//REMEMBER TO CLOSE FILES
 *
 */
