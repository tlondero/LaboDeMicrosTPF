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

#include "DAC_Wrapper.h"

char* concat(const char *s1, const char *s2);

void volume(void *buf, uint32_t size, uint16_t vol);

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
	bool finished = false;
	while (!finished) {
		if (getJustIn()) {

			if (MP3LoadFile("test_tomi.mp3", "test_tomi.wav")) {
				int i = 0;
				if (MP3GetTagData(&ID3Data)) {
					printf("\nSONG INFO\n");
					printf("TITLE: %s\n", ID3Data.title);
					printf("ARTIST: %s\n", ID3Data.artist);
					printf("ALBUM: %s\n", ID3Data.album);
					printf("TRACK NUM: %s\n", ID3Data.trackNum);
					printf("YEAR: %s\n", ID3Data.year);
				}

				DAC_Wrapper_Init();
				DAC_Wrapper_Loop(false);
				DAC_Wrapper_Start_Trigger();

				uint16_t sr_ = 44100;
				MP3_Set_Sample_Rate(sr_);

				while (true) {

#ifdef DEBUG_PRINTF_INFO
					printf("\n[APP] Frame %d decoding started.\n", i);
#endif

					mp3_decoder_result_t res = MP3GetDecodedFrame(buffer,
					MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);

					if (res == MP3DECODER_NO_ERROR) {
						MP3GetLastFrameData(&frameData);

						if (sr_ != frameData.sampleRate) {
							sr_ = frameData.sampleRate;
							MP3_Set_Sample_Rate(sr_);
						}

						//volume(&buffer, frameData.sampleCount, 1000);

						if (DAC_Wrapper_Is_Transfer_Done() || i < 1) {
							DAC_Wrapper_Set_Data_Array(&buffer,
									frameData.sampleCount);
						}

						i++;

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
					} else {
						int huevo = 0;
						huevo++;
					}
				}
			}
		}
	}
	close_file_wav();
//REMEMBER TO CLOSE FILES
	while (1) {
	};
	return 0;
}

char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void volume(void *buf, uint32_t size, uint16_t vol) {
	uint32_t *buf_ = (uint32_t*) buf;
	uint32_t i;
	for (i = 0; i < size; i++) {
		buf_[i] = buf_[i] * vol;
	}
}
