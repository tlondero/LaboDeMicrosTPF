
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

/*******************************************************************************
 * VARIABLE DECLARATION WITH FILE SCOPE
 ******************************************************************************/




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
/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE SCOPE
 ******************************************************************************/

/*******************************************************************************
 *						 INTERRUPTION ROUTINES
 ******************************************************************************/


