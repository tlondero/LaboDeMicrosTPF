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
#include "Equaliser.h"

#define MAX_DAC_VALUE		4095.0
#define MP3_MIN_VALUE		32768
#define MP3_GAP_VALUE		65535.0

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

static uint16_t u_buffer_1[MP3_DECODED_BUFFER_SIZE];
static uint16_t u_buffer_2[MP3_DECODED_BUFFER_SIZE];

//static float32_t eq_buffer[MP3_DECODED_BUFFER_SIZE];

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

			if (MP3LoadFile("test_500.mp3", "test_500.wav")) {
				//if (MP3LoadFile("dakiti.mp3", "dakiti.wav")) {
				int i = 0;
				if (MP3GetTagData(&ID3Data)) {
					printf("\nSONG INFO\n");
					printf("TITLE: %s\n", ID3Data.title);
					printf("ARTIST: %s\n", ID3Data.artist);
					printf("ALBUM: %s\n", ID3Data.album);
					printf("TRACK NUM: %s\n", ID3Data.trackNum);
					printf("YEAR: %s\n", ID3Data.year);
				}

				//eqInit(MP3_DECODED_BUFFER_SIZE);

				DAC_Wrapper_Init();
				DAC_Wrapper_Loop(false);
				DAC_Wrapper_Start_Trigger();

				//Empiezo por el buffer 1
				mp3_decoder_result_t res = MP3GetDecodedFrame(
						(int16_t*) u_buffer_1, MP3_DECODED_BUFFER_SIZE,
						&sampleCount, 0);

				//eqSetFrameSize(frameData.sampleCount);
				//eqFilterFrame(u_buffer_1, frameData.sampleCount, eq_buffer);
				uint16_t j;
				for (j = 0; j < frameData.sampleCount; j++) {
					u_buffer_1[j] = (uint16_t) ((u_buffer_1[j] + 32768) * 4095
							/ 65535.0);
				}

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

							if (using_buffer_1) {
								//Envio el buffer 1 al dac
								DAC_Wrapper_Set_Data_Array(&u_buffer_1,
										frameData.sampleCount);
								DAC_Wrapper_Set_Next_Buffer(&u_buffer_2);

								//Cargo el y normalizo el buffer 2
								res = MP3GetDecodedFrame((int16_t*) u_buffer_2,
								MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);

								//eqSetFrameSize(frameData.sampleCount);
								//eqFilterFrame(u_buffer_2, frameData.sampleCount, eq_buffer);
								for (j = 0; j < frameData.sampleCount; j++) {
									u_buffer_2[j] = (uint16_t) ((u_buffer_2[j]
											+ 32768) * 4095 / 65535.0);
								}
							} else {
								//Envio el buffer 2 al dac
								DAC_Wrapper_Set_Data_Array(&u_buffer_2,
										frameData.sampleCount);
								DAC_Wrapper_Set_Next_Buffer(&u_buffer_1);

								//Cargo el y normalizo el buffer 1
								res = MP3GetDecodedFrame((int16_t*) u_buffer_1,
								MP3_DECODED_BUFFER_SIZE, &sampleCount, 0);

								//eqSetFrameSize(frameData.sampleCount);
								//eqFilterFrame(u_buffer_1, frameData.sampleCount, eq_buffer);
								for (j = 0; j < frameData.sampleCount; j++) {
									u_buffer_1[j] = (uint16_t) ((u_buffer_1[j]
											+ 32768) * 4095 / 65535.0);
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
					} else {
						int huevo = 0;
						huevo++;
					}
				}
			}
		}
	}
	DAC_Wrapper_Clear_Data_Array();
	close_file_wav();
//REMEMBER TO CLOSE FILES
	printf("\nHasta la proxima\n");
	return 0;
}

char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
