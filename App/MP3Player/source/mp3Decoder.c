#include "mp3Decoder.h"

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>
#include <stdbool.h>  
#include "mp3Decoder.h"
#include "debug_ifdefs.h"
#include "lib/helix/pub/mp3dec.h"
#include "lib/id3tagParser/read_id3.h"
#include "stdio.h"

#include "ff.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MP3DECODER_MODE_NORMAL  0
#define MP3_FRAME_BUFFER_BYTES  (10240)//6913            // MP3 buffer size (in bytes)
#define DEFAULT_ID3_FIELD       "Unknown"
#define MAX_DEPTH       3
static FIL file __attribute__((aligned((8U))));	// MP3 file
static FIL *mp3File = &file;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
	// Helix structures

	HMP3Decoder Decoder;                           // Helix MP3 decoder instance
	MP3FrameInfo last_frame_info;                      // current MP3 frame info



	uint32_t f_size;                                       // file size
	uint32_t bytes_remaining; // Encoded MP3 bytes remaining to be processed by either offset or decodeMP3
	bool file_opened;                          // true if there is a loaded file
	uint16_t last_frame_length;                             // Last frame length

	// MP3-encoded buffer
	uint8_t encoded_frame_buffer[MP3_FRAME_BUFFER_BYTES]; // buffer for MP3-encoded frames
	uint32_t top_index; // current position in frame buffer (points to top_index)
	uint32_t bottom_index;       // current position at info end in frame buffer

	// ID3 tag
	bool has_ID3_Tag;                      // True if the file has valid ID3 tag
	mp3_decoder_tag_data_t ID3_data;                 // Parsed data from ID3 tag

} mp3_decoder_context_t;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//File management functions

static bool open_file(const char *file_name);
static void close_file(void);

static void fileSeek(size_t pos);
uint32_t getFileSize(void);
void fileRewind(void);
uint16_t readFile(void *buf, uint16_t cnt);

//read ID3
void readID3Tag(void);

//data management
static void copyDataAndMovePointer(void);
void copyFrameInfo(mp3_decoder_frame_data_t *mp3_data, MP3FrameInfo *helix_data);
void resetContextData(void);
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static mp3_decoder_context_t context_data;

static FRESULT fr;

/*******************************************************************************
 *******************************************************************************
 GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void MP3DecoderInit(void) {
	resetContextData();
	context_data.Decoder = MP3InitDecoder();                //Helix decoder init

}

bool MP3LoadFile(const char *file_name) {
	bool res = false;
	if (context_data.file_opened == true) { //if there was a opened file, i must close it before opening a new one
		resetContextData();
		close_file();
	}
	if (open_file(_T(file_name))) {
		context_data.file_opened = true;
		context_data.f_size = getFileSize();
		context_data.bytes_remaining = context_data.f_size;
		readID3Tag();
		copyDataAndMovePointer();
		res = true;
	}
	return res;

}

bool MP3GetTagData(mp3_decoder_tag_data_t *data) {
	bool res = false;
	if (context_data.has_ID3_Tag) {
		strcpy(data->album, context_data.ID3_data.album);
		strcpy(data->artist, context_data.ID3_data.artist);
		strcpy(data->title, context_data.ID3_data.title);
		strcpy(data->trackNum, context_data.ID3_data.trackNum);
		strcpy(data->year, context_data.ID3_data.year);
		res = true;
	}
	return res;
}

bool MP3GetLastFrameData(mp3_decoder_frame_data_t *data) {
	bool ret = false;
	if (context_data.bytes_remaining < context_data.f_size) {
		copyFrameInfo(data, &context_data.last_frame_info);
		ret = true;
	}
	return ret;
}



mp3_decoder_result_t MP3GetDecodedFrame(short *outBuffer, uint16_t bufferSize,
		uint16_t *samples_decoded) {
	mp3_decoder_result_t ret = MP3DECODER_NO_ERROR; // Return value of the function

		if (!context_data.file_opened) {
			ret = MP3DECODER_NO_FILE;
		} else if (context_data.bytes_remaining) // check if there is remaining info to be decoded
		{
			if ((context_data.top_index > 0)
					&& ((context_data.bottom_index - context_data.top_index) > 0)
					&& (context_data.bottom_index - context_data.top_index
							< MP3_FRAME_BUFFER_BYTES)) {
				memmove(context_data.encoded_frame_buffer,
						context_data.encoded_frame_buffer
								+ context_data.top_index,
						context_data.bottom_index - context_data.top_index);
				context_data.bottom_index = context_data.bottom_index
						- context_data.top_index;
				context_data.top_index = 0;


			}
			copyDataAndMovePointer();


			int offset = MP3FindSyncWord(
					context_data.encoded_frame_buffer + context_data.top_index,
					context_data.bottom_index);

			if (offset >= 0) {
				context_data.top_index += offset; // updating top_index pointer
				context_data.bytes_remaining -= offset; // and update the bytes remaining
			}

			MP3FrameInfo nextFrameInfo;
			int err = MP3GetNextFrameInfo(context_data.Decoder, &nextFrameInfo,
					context_data.encoded_frame_buffer + context_data.top_index);


			// decode a frame
			uint8_t *decPointer = context_data.encoded_frame_buffer
					+ context_data.top_index;
			int bytesLeft = context_data.bottom_index - context_data.top_index;
			int res = MP3Decode(context_data.Decoder, &decPointer, &(bytesLeft),
					outBuffer, MP3DECODER_MODE_NORMAL);

			if (res == ERR_MP3_NONE) // if decoding successful
					{
				uint16_t decodedBytes = context_data.bottom_index
						- context_data.top_index - bytesLeft;
				context_data.last_frame_length = decodedBytes;

				// update header pointer and file size
				context_data.top_index += decodedBytes;
				context_data.bytes_remaining -= decodedBytes;

				// update last frame decoded info
				MP3GetLastFrameInfo(context_data.Decoder,
						&(context_data.last_frame_info));

				// update samples decoded
				*samples_decoded = context_data.last_frame_info.outputSamps;

				// return success code
				ret = MP3DECODER_NO_ERROR;
			}
		} else {
			ret = MP3DECODER_FILE_END;
		}

	return ret;

}

void printContextData(void) {
	printf("Bottom index %d\r\n", context_data.bottom_index);
	printf("Top index %d\r\n", context_data.top_index);
	printf("Bytes remaining %d\r\n", context_data.bytes_remaining);
	printf("Dir ptr %p\r\n", file.dir_ptr);
	printf("f ptr %p\r\n", file.fptr);
}

/*******************************************************************************
 *******************************************************************************
 LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

//File management functions


static bool open_file(const char *file_name) {
	bool ret = false;

	fr = f_open(&file, _T(file_name), FA_READ);
	if (fr == FR_OK) {
		mp3File = &(file);
		ret = true;
	}

	return ret;
}

static void close_file(void) {

	f_close(mp3File);

}


static void fileSeek(size_t pos) {
	f_lseek(mp3File, pos);

}

uint32_t getFileSize(void) {
	uint32_t ret = 0;
	if (context_data.file_opened) {

		ret = f_size(mp3File);

	}
	return ret;
}

void fileRewind(void) {

	f_rewind(mp3File);

}

uint16_t readFile(void *buf, uint16_t cnt) {
	uint16_t ret = 0;
	UINT read;
	if (context_data.file_opened) {


		fr = f_read(&file, ((uint8_t*) buf), cnt, &read);
		if (fr == FR_OK) {
			ret = read;
		}

	}
	return ret;
}

//ID3
void readID3Tag(void) {

	if (has_ID3_tag(mp3File)) {
		context_data.has_ID3_Tag = true;

		if (!read_ID3_info(TITLE_ID3, context_data.ID3_data.title,
				ID3_MAX_FIELD_SIZE, mp3File))
			strcpy(context_data.ID3_data.title, DEFAULT_ID3_FIELD);

		if (!read_ID3_info(ALBUM_ID3, context_data.ID3_data.album,
				ID3_MAX_FIELD_SIZE, mp3File))
			strcpy(context_data.ID3_data.album, DEFAULT_ID3_FIELD);

		if (!read_ID3_info(ARTIST_ID3, context_data.ID3_data.artist,
				ID3_MAX_FIELD_SIZE, mp3File))
			strcpy(context_data.ID3_data.artist, DEFAULT_ID3_FIELD);

		if (!read_ID3_info(YEAR_ID3, context_data.ID3_data.year, 10, mp3File))
			strcpy(context_data.ID3_data.year, DEFAULT_ID3_FIELD);

		if (!read_ID3_info(TRACK_NUM_ID3, context_data.ID3_data.trackNum, 10,
				mp3File))
			strcpy(context_data.ID3_data.trackNum, DEFAULT_ID3_FIELD);

		unsigned int tagSize = get_ID3_size(mp3File);


		fileSeek(tagSize);
		context_data.bytes_remaining -= tagSize;
	} else {
		fileRewind();
	}
}

void copyDataAndMovePointer() {
	uint16_t bytes_read;

	// Fill buffer with info in mp3 file
#ifdef DEBUG_ALAN
    uint8_t auxbuff[5000];
    if (MP3_FRAME_BUFFER_BYTES - context_data.bottom_index > 0){
    	if(context_data.bottom_index == 0){
    	    uint8_t* dst = context_data.encoded_frame_buffer + context_data.bottom_index;
    		bytes_read = readFile(dst, (MP3_FRAME_BUFFER_BYTES - context_data.bottom_index));
    		// Update bottom_index pointer
    		context_data.bottom_index += bytes_read;
    	}
    	else{
    	    uint8_t* dst = auxbuff;
    		bytes_read = readFile(dst, (MP3_FRAME_BUFFER_BYTES - context_data.bottom_index));
    		memmove(context_data.encoded_frame_buffer + context_data.bottom_index, auxbuff, MP3_FRAME_BUFFER_BYTES - context_data.bottom_index);
    		context_data.bottom_index += bytes_read;
    	}
    }
#endif
#ifndef DEBUG_ALAN
	uint8_t *dst = context_data.encoded_frame_buffer
			+ context_data.bottom_index;
	if (MP3_FRAME_BUFFER_BYTES - context_data.bottom_index > 0) {
		bytes_read = readFile(dst,
				(MP3_FRAME_BUFFER_BYTES - context_data.bottom_index));
		// Update bottom_index pointer
		context_data.bottom_index += bytes_read;
	}
#endif

	if (bytes_read == 0) {
#ifdef DEBUG_PRINTF_APP
		printf("[App] File was read completely.\n");
#endif
	}

#ifdef DEBUG_PRINTF_INFO
    printf("Read %d bytes from file. Head = %d - Bottom = %d\n", bytes_read, context_data.top_index, context_data.bottom_index);
#endif
}

void copyFrameInfo(mp3_decoder_frame_data_t *mp3_data, MP3FrameInfo *helix_data) {
	mp3_data->bitRate = helix_data->bitrate;
	mp3_data->binitsPerSample = helix_data->bitsPerSample;
	mp3_data->channelCount = helix_data->nChans;
	mp3_data->sampleRate = helix_data->samprate;
	mp3_data->sampleCount = helix_data->outputSamps;
}
void resetContextData(void) {
	context_data.file_opened = false;
	mp3File = NULL;
	context_data.has_ID3_Tag = false;
	context_data.bottom_index = 0;
	context_data.top_index = 0;
	context_data.bytes_remaining = 0;
	context_data.f_size = 0;
}

