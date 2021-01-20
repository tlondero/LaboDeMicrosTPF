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
#include "fsl_sd_disk.h"
#include "fsl_common.h"
#include "fsl_pit.h"
#include "SD_Detect_Wraper.h"
#include "mp3Decoder.h"
#include "ff.h"

char* concat(const char *s1, const char *s2);

void cbackin(void) {
#ifdef DEBUG_PRINTF_APP
	printf("[App] SD Card inserted.\r\n");
#endif
	MP3DecoderInit();
}
void cbackout(void) {
#ifdef DEBUG_PRINTF_APP
	printf("[App] SD Card removed.\r\n");
#endif
}

#ifdef DEBUG_FRAME_DELAY
bool nextFrameFlag = false;
#endif
static short buffer[MP3_DECODED_BUFFER_SIZE];
mp3_decoder_frame_data_t frameData;
mp3_decoder_tag_data_t ID3Data;

int main(void) {
	uint16_t sampleCount;
	uint32_t wrote = 0;
#ifdef DEBUG_PRINTF_INFO
	uint32_t sr = 0;
#endif

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL

	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
	SDWraperInit(cbackin, cbackout);
/////////////////////////////////////////////////////////////
	while (1) {
		if (getJustIn()) {

			if (MP3LoadFile("test.mp3", "test.wav")) {
				int i = 0;
				if (MP3GetTagData(&ID3Data)) {
											printf("\nSONG INFO\n");
											printf("TITLE: %s\n", ID3Data.title);
											printf("ARTIST: %s\n", ID3Data.artist);
											printf("ALBUM: %s\n", ID3Data.album);
											printf("TRACK NUM: %s\n", ID3Data.trackNum);
											printf("YEAR: %s\n", ID3Data.year);
				}

				while (1) {


#ifdef DEBUG_PRINTF_INFO
					printf("\n[APP] Frame %d decoding started.\n", i);
#endif

					mp3_decoder_result_t res = MP3GetDecodedFrame(buffer,
							MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);

					if (res == 0) {
						MP3GetLastFrameData(&frameData);
						wrote = storeWavInSd(&frameData, buffer); //TODO ver por que no anda

						i++;
						if(i == 1000){
							i++;
							i--;
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

					}
					else if (res == MP3DECODER_FILE_END) {
#ifdef DEBUG_PRINTF_APP
						printf("[APP] FILE ENDED. Decoded %d frames.\n", i - 1);
#endif

						break;
					}
					else {
						int huevo = 0;
						huevo++;
					}
				}
			}
		}
	}

	return 0;
}

char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
