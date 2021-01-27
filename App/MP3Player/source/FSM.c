
/***************************************************************************/ /**
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
#include "mp3Decoder.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define DEBUG_PRINTF_APP
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

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
void FSM_menu(event_t *ev, app_context_t *appContext){

	if (SDWRAPPER_getJustOut()) { /* If SD is removed */
		if (appContext->menuState == kAPP_MENU_FILESYSTEM) { /* and menu is exploring FS*/
			appContext->menuState = kAPP_MENU_MAIN; /* Go back to main menu */
			//TODO stop music, stop spectogram
			//...
			switchAppState(appContext->appState, kAPP_STATE_IDDLE); /* Return to iddle state */

		}
	}
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
			}
#ifdef DEBUG_PRINTF_APP
			printf("[App] Went back a directory\n");
			printf("[App] Pointing currently to: %s\n",
					appContext->currentFile);
#endif
		}
		if (ev->btn_evs.pause_play_button) {
			appContext->playerContext.songPaused = true;
			switchAppState(appContext->appState, kAPP_STATE_IDDLE);
#ifdef DEBUG_PRINTF_APP
			printf("[App] Stopped playing.\n");
#endif
		}
		if (ev->fsexp_evs.play_music) {
#ifdef DEBUG_PRINTF_APP
			printf("[App] Switched to playing: %s\n",
					FSEXP_getMP3Path());
#endif
			//TODO: Reiniciar todo para reproducir la proxima cancion.
		}
		if (appContext->playerContext.songEnded) {
			switchAppState(appContext->appState, kAPP_STATE_IDDLE);
		}
	}



	if (appContext->playerContext.songEnded) {
		appContext->playerContext.songEnded = false;
	} else {
		runPlayer(ev, appContext); /* Run player in background */
	}

}

uint16_t * getbuffer1(void){
	return u_buffer_1;
}
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/
void runPlayer(event_t *events, app_context_t *appContext) {
	//TODO: Implementar aca el reproductor y el espectrograma (ver si no van a funcar a interrupciones tho)

	if (appContext->playerContext.songPaused) {
		DAC_Wrapper_Clear_Data_Array();
		appContext->playerContext.songPaused = false;
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
						appContext->playerContext.volume);
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
						appContext->playerContext.volume);
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


