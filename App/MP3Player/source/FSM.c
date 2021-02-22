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
#include "fft.h"
#include "HAL/delays.h"
#include "fsl_rtc.h"
#include "equalizer.h"
#include "Equaliser_Presets.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define USECS_MIN 60000000U

typedef float32_t REAL;
#define NPOLE 2
#define NZERO 2
REAL acoeff[] = { 0.33336499651457274, -0.9428985937980157, 1 };
REAL bcoeff[] = { 1, 2, 1 };
REAL gain = 10.244159221308562;
REAL xv[] = { 0, 0, 0 };
REAL yv[] = { 0, 0, 0 };
REAL applyfilter(REAL v);

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum {
	FS, EQ, SPECT, DATE, SUBMENU_CANT
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
enum {
	YEAR_D, MONTH_D, DAY_D, HOUR_D, MINUTE_D, SUB_MENU_D_CANT
};
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH FILE SCOPE
 ******************************************************************************/
void adaptFFT(int16_t *src, float32_t *dst, uint16_t cnt);
void translateBinesToMatrix(float32_t *bines);
/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/
static uint16_t u_buffer_1[MP3_DECODED_BUFFER_SIZE];
static uint16_t u_buffer_2[MP3_DECODED_BUFFER_SIZE];
static char packPath[MAX_PATH_LENGHT + 5];

static rtc_datetime_t date;

#define FFT_LEN 512
static float32_t u_buffer_fft[FFT_LEN * 2];
static float32_t buffer_fft_calculated_mag[FFT_LEN];
static float32_t fft_8_bines[8];

static char time_string[20];

bool eqSelectOff = true;
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH GLOBAL SCOPE
 ******************************************************************************/
void FSM_init() {
	time_string[0]='1';
	time_string[1]='2';
	time_string[6]='/';
	time_string[9]='/';
	time_string[14]=':';
	time_string[17]='\r';
	time_string[18]='\n';
	time_string[19]='\0';

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_3,
			USEC_TO_COUNT(USECS_MIN, CLOCK_GetFreq(kCLOCK_BusClk)));

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_3, kPIT_TimerInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(PIT3_IRQn);
	PIT_StartTimer(PIT, kPIT_Chnl_3);
}

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
				UART_WriteBlocking(UART0,
						(uint8_t*) "00File System Explorer Menu\r\n", 30);
#endif
				break;

			case EQ:
#ifdef DEBUG_PRINTF_APP
				printf("Equalizer Menu\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "00Equalizer Menu\r\n",
						19);

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
					UART_WriteBlocking(UART0, (uint8_t*) "00Spectrogram on\r\n",
							19);

				} else {
					UART_WriteBlocking(UART0,
							(uint8_t*) "00Spectrogram off\r\n", 20);
				}
#endif
				break;
			case DATE:
#ifdef DEBUG_PRINTF_APP
				printf("Date Menu\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "00Date Menu\r\n", 14);

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
				UART_WriteBlocking(UART0,
						(uint8_t*) "00File System Explorer Menu\r\n", 30);
#endif
				break;
			case EQ:
#ifdef DEBUG_PRINTF_APP
				printf("Equalizer Menu\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "00Equalizer Menu\r\n",
						19);
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
					UART_WriteBlocking(UART0, (uint8_t*) "00Spectrogram on\r\n",
							19);
				} else {
					UART_WriteBlocking(UART0,
							(uint8_t*) "00Spectrogram off\r\n", 20);
				}
#endif
				break;
			case DATE:
#ifdef DEBUG_PRINTF_APP
				printf("Date Menu\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "00Date Menu\r\n", 14);
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.enter_button) {
			switch (index % SUBMENU_CANT) {
			case FS:
				if(SDWRAPPER_getSDInserted()){
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
				UART_WriteBlocking(UART0, (uint8_t*) packPath, i);

#endif
				appContext->menuState = kAPP_MENU_FILESYSTEM;
				}
				else{
					UART_WriteBlocking(UART0, "06\r\n", 5);
				}
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
					UART_WriteBlocking(UART0, (uint8_t*) "00Spectrogram on\r\n",
							19);
#endif
				} else {
					LEDMATRIX_Pause();
#ifdef DEBUG_PRINTF_APP
					printf("Spectrogram off\r\n");
#else
					UART_WriteBlocking(UART0,
							(uint8_t*) "00Spectrogram off\r\n", 20);
#endif
				}
				break;
			case DATE:
#ifdef DEBUG_PRINTF_APP
				printf("Date Menu opened\r\n");
#endif
				appContext->menuState = kAPP_MENU_DATE;
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
				UART_WriteBlocking(UART0, (uint8_t*) packPath, i);

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
				UART_WriteBlocking(UART0, (uint8_t*) packPath, i);

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
				UART_WriteBlocking(UART0, (uint8_t*) packPath, i);

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
					UART_WriteBlocking(UART0, (uint8_t*) packPath, i);
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
				UART_WriteBlocking(UART0, (uint8_t*) "11OFF\r\n", 8);
#endif
				break;
			case CLASSIC:
#ifdef DEBUG_PRINTF_APP
				printf("CLASSIC preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11CLASSIC\r\n", 12);
#endif
				break;
			case CLUB:
#ifdef DEBUG_PRINTF_APP
							printf("CLUB preset\r\n");
			#else
				UART_WriteBlocking(UART0, (uint8_t*) "11CLUB\r\n", 9);
#endif
				break;
			case DANCE:
#ifdef DEBUG_PRINTF_APP
				printf("DANCE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11DANCE\r\n", 10);
#endif
				break;
			case BASS:
#ifdef DEBUG_PRINTF_APP
				printf("BASS preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11BASS\r\n", 9);
#endif
				break;
			case BASS_AND_TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("BASS_AND_TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11BASS_AND_TREBLE\r\n",
						20);
#endif
				break;
			case TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11TREBLE\r\n", 11);
#endif
				break;
			case HEADSET:
#ifdef DEBUG_PRINTF_APP
				printf("HEADSET preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11HEADSET\r\n", 12);
#endif
				break;
			case HALL:
#ifdef DEBUG_PRINTF_APP
				printf("HALL preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11HALL\r\n", 9);
#endif
				break;
			case LIVE:
#ifdef DEBUG_PRINTF_APP
				printf("LIVE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11LIVE\r\n", 9);
#endif
				break;
			case PARTY:
#ifdef DEBUG_PRINTF_APP
				printf("PARTY preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11PARTY\r\n", 10);
#endif
				break;
			case POP:
#ifdef DEBUG_PRINTF_APP
				printf("POP preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11POP\r\n", 8);
#endif
				break;
			case REGGAE:
#ifdef DEBUG_PRINTF_APP
				printf("REGGAE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11REGGAE\r\n", 11);
#endif
				break;
			case ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11ROCK\r\n", 9);
#endif
				break;
			case SKA:
#ifdef DEBUG_PRINTF_APP
				printf("SKA preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11SKA\r\n", 8);
#endif
				break;
			case SOFT:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11SOFT\r\n", 9);
#endif
				break;
			case SOFT_ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT_ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11SOFT_ROCK\r\n", 14);
#endif
				break;
			case TECHNO:
#ifdef DEBUG_PRINTF_APP
				printf("TECHNO preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11TECHNO\r\n", 11);
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
				UART_WriteBlocking(UART0, (uint8_t*) "11OFF\r\n", 8);
#endif
				break;
			case CLASSIC:
#ifdef DEBUG_PRINTF_APP
				printf("CLASSIC preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11CLASSIC\r\n", 12);
#endif
				break;
			case CLUB:
#ifdef DEBUG_PRINTF_APP
							printf("CLUB preset\r\n");
			#else
				UART_WriteBlocking(UART0, (uint8_t*) "11CLUB\r\n", 9);
#endif
				break;
			case DANCE:
#ifdef DEBUG_PRINTF_APP
				printf("DANCE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11DANCE\r\n", 10);
#endif
				break;
			case BASS:
#ifdef DEBUG_PRINTF_APP
				printf("BASS preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11BASS\r\n", 9);
#endif
				break;
			case BASS_AND_TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("BASS_AND_TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11BASS_AND_TREBLE\r\n",
						20);
#endif
				break;
			case TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("TREBLE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11TREBLE\r\n", 11);
#endif
				break;
			case HEADSET:
#ifdef DEBUG_PRINTF_APP
				printf("HEADSET preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11HEADSET\r\n", 12);
#endif
				break;
			case HALL:
#ifdef DEBUG_PRINTF_APP
				printf("HALL preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11HALL\r\n", 9);
#endif
				break;
			case LIVE:
#ifdef DEBUG_PRINTF_APP
				printf("LIVE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11LIVE\r\n", 9);
#endif
				break;
			case PARTY:
#ifdef DEBUG_PRINTF_APP
				printf("PARTY preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11PARTY\r\n", 10);
#endif
				break;
			case POP:
#ifdef DEBUG_PRINTF_APP
				printf("POP preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11POP\r\n", 8);
#endif
				break;
			case REGGAE:
#ifdef DEBUG_PRINTF_APP
				printf("REGGAE preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11REGGAE\r\n", 11);
#endif
				break;
			case ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11ROCK\r\n", 9);
#endif
				break;
			case SKA:
#ifdef DEBUG_PRINTF_APP
				printf("SKA preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11SKA\r\n", 8);
#endif
				break;
			case SOFT:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11SOFT\r\n", 9);
#endif
				break;
			case SOFT_ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("SOFT_ROCK preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11SOFT_ROCK\r\n", 14);
#endif
				break;
			case TECHNO:
#ifdef DEBUG_PRINTF_APP
				printf("TECHNO preset\r\n");
#else
				UART_WriteBlocking(UART0, (uint8_t*) "11TECHNO\r\n", 11);
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.enter_button) {
			eqSelectOff = false;
			switch (index % CANT_PRESETS) {
			//OFF,CLASSIC,CLUB,DANCE,BASS,BASS_AND_TREBLE , TREBLE, HEADSET,HALL,LIVE,PARTY,POP,REGGAE,ROCK,SKA,SOFT,SOFT_ROCK,TECHNO
			case OFF:
#ifdef DEBUG_PRINTF_APP
				printf("OFF preset Selected\r\n");
#endif
				eqSelectOff = true;
				equalizer_change_effect(eq_off_db);
				break;
			case CLASSIC:
#ifdef DEBUG_PRINTF_APP
				printf("CLASSIC preset Selected\r\n");
#endif
				equalizer_change_effect(eq_clasic_db);
				break;
			case CLUB:
#ifdef DEBUG_PRINTF_APP
				printf("CLUB preset Selected\r\n");
#endif
				equalizer_change_effect(eq_club_db);
				break;
			case DANCE:
#ifdef DEBUG_PRINTF_APP
				printf("DANCE preset Selected\r\n");
#endif
				equalizer_change_effect(eq_dance_db);
				break;
			case BASS:
#ifdef DEBUG_PRINTF_APP
				printf("BASS preset Selected\r\n");
#endif
				equalizer_change_effect(eq_bass_db);
				break;
			case BASS_AND_TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("BASS_AND_TREBLE preset Selected\r\n");
#endif
				equalizer_change_effect(eq_bass_and_treble_db);
				break;
			case TREBLE:
#ifdef DEBUG_PRINTF_APP
				printf("TREBLE preset Selected\r\n");
#endif
				equalizer_change_effect(eq_trable_db);
				break;
			case HEADSET:
#ifdef DEBUG_PRINTF_APP
				printf("HEADSET preset Selected\r\n");
#endif
				equalizer_change_effect(eq_headset_db);
				break;
			case HALL:
#ifdef DEBUG_PRINTF_APP
				printf("HALL preset Selected\r\n");
#endif
				equalizer_change_effect(eq_hall_db);
				break;
			case LIVE:
#ifdef DEBUG_PRINTF_APP
				printf("LIVE preset Selected\r\n");
#endif
				equalizer_change_effect(eq_live_db);
				break;
			case PARTY:
#ifdef DEBUG_PRINTF_APP
				printf("PARTY preset Selected\r\n");
#endif
				equalizer_change_effect(eq_party_db);
				break;
			case POP:
#ifdef DEBUG_PRINTF_APP
				printf("POP preset Selected\r\n");
#endif
				equalizer_change_effect(eq_pop_db);
				break;
			case REGGAE:
#ifdef DEBUG_PRINTF_APP
				printf("REGGAE preset Selected\r\n");
#endif
				equalizer_change_effect(eq_reggae_db);
				break;
			case ROCK:
#ifdef DEBUG_PRINTF_APP
				printf("ROCK preset Selected\r\n");
#endif
				equalizer_change_effect(eq_rock_db);
				break;
			case SKA:
#ifdef DEBUG_PRINTF_APP
				printf("SKA preset Selected\r\n");
#endif
				equalizer_change_effect(eq_ska_db);
				break;
			case SOFT:
#ifdef DEBUG_PRINTF_APP
							printf("SOFT preset Selected\r\n");
			#endif
				equalizer_change_effect(eq_soft_db);
				break;
			case SOFT_ROCK:
#ifdef DEBUG_PRINTF_APP
							printf("SOFT_ROCK preset Selected\r\n");
			#endif
				equalizer_change_effect(eq_soft_rock_db);
				break;
			case TECHNO:
#ifdef DEBUG_PRINTF_APP
							printf("TECHNO preset Selected\r\n");
			#endif
				equalizer_change_effect(eq_techno_db);
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
	} else if (appContext->menuState == kAPP_MENU_DATE) {
		uint16_t aux_m, aux_c, aux_d, aux_u;
		static uint16_t y = 2021;
		static uint8_t mo = 2;
		static uint8_t d = 22;
		static uint8_t h = 11;
		static uint8_t min = 0;
		static uint8_t men_index = 0;
		char msg[10] = "13A2021\r\n";
		if (ev->btn_evs.next_button) {
			switch (men_index % SUB_MENU_D_CANT) {
			case YEAR_D:
				y++;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'A';

				aux_m = y / 1000;
				msg[3] = aux_m + '0';
				aux_c = (y - aux_m * 1000) / 100;

				msg[4] = aux_c + '0';
				aux_d = (y - aux_m * 1000 - aux_c * 100) / 10;
				msg[5] = aux_d + '0';
				aux_u = (y - aux_m * 1000 - aux_c * 100 - aux_d * 10);
				msg[6] = (aux_u) + '0';
				msg[7] = '\r';
				msg[8] = '\n';
				msg[9] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 10);
#endif
				break;
			case MONTH_D:
				if (mo < 12)
					mo++;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'M';
				aux_d = mo / 10;
				msg[3] = aux_d + '0';
				aux_u = (mo - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			case DAY_D:
				if (d < 31)
					d++;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'D';
				aux_d = d / 10;
				msg[3] = aux_d + '0';
				aux_u = (d - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			case HOUR_D:
				if (h < 23)
					h++;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'H';
				aux_d = h / 10;
				msg[3] = aux_d + '0';
				aux_u = (h - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			case MINUTE_D:
				if (min < 59)
					min++;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'm';
				aux_d = min / 10;
				msg[3] = aux_d + '0';
				aux_u = (min - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.prev_button) {
			switch (men_index % SUB_MENU_D_CANT) {
			case YEAR_D:
				y--;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'A';
				aux_m = y / 1000;
				msg[3] = aux_m + '0';
				aux_c = (y - aux_m * 1000) / 100;

				msg[4] = aux_c + '0';
				aux_d = (y - aux_m * 1000 - aux_c * 100) / 10;
				msg[5] = aux_d + '0';
				aux_u = (y - aux_m * 1000 - aux_c * 100 - aux_d * 10);
				msg[6] = (aux_u) + '0';
				msg[7] = '\r';
				msg[8] = '\n';
				msg[9] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 10);
#endif
				break;
			case MONTH_D:
				if (mo > 1)
					mo--;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'M';
				aux_d = mo / 10;
				msg[3] = aux_d + '0';
				aux_u = (mo - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			case DAY_D:
				if (d > 1)
					d--;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'D';
				aux_d = d / 10;
				msg[3] = aux_d + '0';
				aux_u = (d - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			case HOUR_D:
				if (h > 0)
					h--;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'H';
				aux_d = h / 10;
				msg[3] = aux_d + '0';
				aux_u = (h - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			case MINUTE_D:
				if (min > 0)
					min--;
#ifndef DEBUG_PRINTF_APP
				msg[2] = 'm';
				aux_d = min / 10;
				msg[3] = aux_d + '0';
				aux_u = (min - aux_d * 10);
				msg[4] = aux_u + '0';
				msg[5] = '\r';
				msg[6] = '\n';
				msg[7] = '\0';
				UART_WriteBlocking(UART0, (uint8_t*) msg, 8);
#endif
				break;
			default:
				break;
			}
		} else if (ev->btn_evs.enter_button) {
			switch (men_index % SUB_MENU_D_CANT) {
			case YEAR_D:
				date.year = y;
				break;
			case MONTH_D:
				date.month = mo;
				break;
			case DAY_D:
				date.day = d;
				break;
			case HOUR_D:
				date.hour = h;
				break;
			case MINUTE_D:
				date.minute = min;
				RTC_StopTimer(RTC);
				RTC_SetDatetime(RTC, &date);
				RTC_StartTimer(RTC);
				UART_WriteBlocking(UART0, (uint8_t*) "13O\r\n", 6);
				appContext->menuState = kAPP_MENU_MAIN;
				men_index=0;
				break;
			default:
				break;
			}
			men_index++;
		} else if (ev->btn_evs.back_button) {

			switch (men_index % SUB_MENU_D_CANT) {
			case YEAR_D:
				appContext->menuState = kAPP_MENU_MAIN;
				break;
			case MONTH_D:
			case DAY_D:
			case HOUR_D:
			case MINUTE_D:
				men_index--;
				break;
			default:
				break;
			}
//			appContext->menuState = kAPP_MENU_MAIN;
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
						&(appContext->playerContext.sampleCount));

				if (!eqSelectOff) {
					equalize_frame((int16_t *)u_buffer_2, (int16_t *)u_buffer_2);
				}

				adaptFFT((int16_t *)u_buffer_2, u_buffer_fft, FFT_LEN);
				fft(u_buffer_fft, u_buffer_fft, 1);
				fftGetMag(u_buffer_fft, buffer_fft_calculated_mag);
				fftMakeBines8(buffer_fft_calculated_mag, fft_8_bines);
				translateBinesToMatrix(&(fft_8_bines[0]));

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
						&(appContext->playerContext.sampleCount));

				if(!eqSelectOff){
					equalize_frame((int16_t *)u_buffer_1, (int16_t *)u_buffer_1);
				}

				adaptFFT((int16_t *)u_buffer_1, u_buffer_fft, FFT_LEN);
				fft(u_buffer_fft, u_buffer_fft, 1);
				fftGetMag(u_buffer_fft, buffer_fft_calculated_mag);
				fftMakeBines8(buffer_fft_calculated_mag, fft_8_bines);
				translateBinesToMatrix(&(fft_8_bines[0]));

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

char* GetPackPath(void) {
	return packPath;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/
void adaptFFT(int16_t *src, float32_t *dst, uint16_t cnt) {
	uint16_t i = 0;
	uint16_t j = 0;
	for (j = 0; j < cnt; i++, j++) {
		dst[2*i+1] = 0;
		dst[i * 2] = src[i] * sinf(PI * i / FFT_LEN) + 0.0f; //HAnn window
	}
}

void translateBinesToMatrix(float32_t *bines) {
	int8_t i = 0, j = 0;
	for (i = 7; i > -1; i--) {
		for (j = 0; j < 8; j++) {
			if (j < bines[7-i] + 1) {
				if (j < 4) {
					if (bines[7-i] == 0) {
						LEDMATRIX_SetLed(j, i, 0, 0, 0);
					}
					LEDMATRIX_SetLed(j - 1, i, 0, 5, 0);
				} else if (j < 6) {
					LEDMATRIX_SetLed(j - 1, i, 1, 1, 0);
				} else {
					LEDMATRIX_SetLed(j - 1, i, 5, 0, 0);
				}
			} else {
				LEDMATRIX_SetLed(j, i, 0, 0, 0);
			}
		}
	}
}

REAL applyfilter(REAL v) {
	int i;
	REAL out = 0;
	for (i = 0; i < NZERO; i++) {
		xv[i] = xv[i + 1];
	}
	xv[NZERO] = v / gain;
	for (i = 0; i < NPOLE; i++) {
		yv[i] = yv[i + 1];
	}
	for (i = 0; i <= NZERO; i++) {
		out += xv[i] * bcoeff[i];
	}
	for (i = 0; i < NPOLE; i++) {
		out -= yv[i] * acoeff[i];
	}
	yv[NPOLE] = out;
	return out;
}

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/
void PIT3_IRQHandler(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);
	RTC_GetDatetime(RTC, &date);
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
	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();

}
