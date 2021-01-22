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

#define MAX_DAC				4095
#define MP3_MAX_VALUE		100000//32767
#define MP3_MIN_VALUE		-100000//-32768
#define MP3_GAP				MP3_MAX_VALUE - MP3_MIN_VALUE

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

static short buffer_work[MP3_DECODED_BUFFER_SIZE];
static short buffer_load[MP3_DECODED_BUFFER_SIZE];

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
	short min = 1;
	short max = 1;
	while (!finished) {
		if (getJustIn()) {

			//if (MP3LoadFile("test_500.mp3", "test_500.wav")) {
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

				mp3_decoder_result_t res_work = MP3GetDecodedFrame(buffer_work,
				MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);
				mp3_decoder_result_t res_load = MP3GetDecodedFrame(buffer_load,
				MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);

				bool sendWork = true;

				while (true) {

#ifdef DEBUG_PRINTF_INFO
					printf("\n[APP] Frame %d decoding started.\n", i);
#endif

					if (res_work == MP3DECODER_NO_ERROR) {

						if (DAC_Wrapper_Is_Transfer_Done()) {

							res_work = res_load;

							MP3GetLastFrameData(&frameData);

							uint16_t j;
							for (j = 0; j < frameData.sampleCount; j++) {
								buffer_work[j] = (uint16_t) ((buffer_load[j]
										- MP3_MIN_VALUE) * MAX_DAC / MP3_GAP);
//								buffer_work[j] = buffer_load[j];
								if (max < buffer_load[j]) {
									max = buffer_load[j];
								}
								if (min > buffer_load[j]) {
									min = buffer_load[j];
								}
							}

							res_load = MP3GetDecodedFrame(buffer_load,
							MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);

							sendWork = true;
						}

						if (sendWork) {

							MP3GetLastFrameData(&frameData);

							if (sr_ != frameData.sampleRate) {
								sr_ = frameData.sampleRate;
								MP3_Set_Sample_Rate(sr_);
							}

							DAC_Wrapper_Set_Data_Array(&buffer_work,
									frameData.sampleCount);
							sendWork = false;

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

					} else if (res_work == MP3DECODER_FILE_END) {
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
	printf("Hasta la proxima \n");
	printf("max = %d \n", max);
	printf("min = %d \n", min);
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