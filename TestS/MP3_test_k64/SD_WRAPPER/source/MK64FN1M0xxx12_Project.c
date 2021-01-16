/**
 * @file    MK64FN1M0xxx12_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
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
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

#define MAIN_DEBUG
char* concat(const char *s1, const char *s2);

static FIL g_fileObject; /* File object */

/*
 * @brief   Application entry point.
 */
void cbackin(void) {
	printf("Atroden pa\r\n");
	MP3DecoderInit();
}
void cbackout(void) {
	printf("Arafue pa\r\n");
}
static short buffer[MP3_DECODED_BUFFER_SIZE];
mp3_decoder_frame_data_t frameData;
mp3_decoder_tag_data_t ID3Data;

int main(void) {
	uint16_t sampleCount;
	uint32_t sr = 0;
	uint8_t j = 0;

	FRESULT error;
	DIR directory; /* Directory object */
	FILINFO fileInformation;
	uint8_t read_buffer[10000];
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
	SDWraperInit(cbackin, cbackout);
	uint16_t bytesWritten=0;
/////////////////////////////////////////////////////////////
	while (1) {
		if (getJustIn()) {
//			error = f_open(&g_fileObject, _T("test.mp3"), FA_READ);
//			bytesWritten = f_size(&g_fileObject);
//			error = f_read(&g_fileObject, buffer, sizeof(buffer), &bytesWritten);
//			int huevo=0;
//			huevo++;
//			huevo--;


			if (MP3LoadFile("test.mp3")) {
				int i = 0;
				while (1) {
#ifdef MAIN_DEBUG
					printf("\n[APP] Frame %d decoding started.\n", i);
#endif
					mp3_decoder_result_t res = MP3GetDecodedFrame(buffer,
							MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);
					if (res == 0) {
						MP3GetLastFrameData(&frameData);
						if (sr != frameData.sampleRate) {
							int huevo = 0;
							huevo++;
						}
#ifdef MAIN_DEBUG
						printf("[APP] Frame %d decoded.\n", i);
#endif
						i++;

						sr = frameData.sampleRate;
#ifdef MAIN_DEBUG
						printf("[APP] FRAME SAMPLE RATE: %d \n", sr);
#endif

						int16_t auxBuffer[MP3_DECODED_BUFFER_SIZE];
						for (uint32_t j = 0;
								j < sampleCount / frameData.channelCount; j++) {
							auxBuffer[j] = buffer[frameData.channelCount * j];
						}
						//wav_write(wav, auxBuffer, sampleCount / frameData.channelCount);
					} else if (res == MP3DECODER_FILE_END) {
						printf("[APP] FILE ENDED. Decoded %d frames.\n", i - 1);
						//wav_close(wav);
						if (MP3GetTagData(&ID3Data)) {
							printf("\nSONG INFO\n");
							printf("TITLE: %s\n", ID3Data.title);
							printf("ARTIST: %s\n", ID3Data.artist);
							printf("ALBUM: %s\n", ID3Data.album);
							printf("TRACK NUM: %s\n", ID3Data.trackNum);
							printf("YEAR: %s\n", ID3Data.year);
						}
						break;
					} else {
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
