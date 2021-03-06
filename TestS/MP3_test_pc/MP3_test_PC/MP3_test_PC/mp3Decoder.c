#include "mp3Decoder.h"

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>
#include <stdbool.h>  
#include "mp3decoder.h"
#include "lib/helix/pub/mp3dec.h"
#include "lib/id3tagParser/read_id3.h"

#ifdef __arm__
#include "lib/fatfs/ff.h"
#endif

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MP3DECODER_MODE_NORMAL  0
#define MP3_FRAME_BUFFER_BYTES  6913            // MP3 buffer size (in bytes)
#define DEFAULT_ID3_FIELD       "Unknown"
#define MAX_DEPTH       3

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct
{
    // Helix structures
    HMP3Decoder   Decoder;                                   // Helix MP3 decoder instance 
    MP3FrameInfo  last_frame_info;                                  // current MP3 frame info

    // MP3 file
#ifdef __arm__
    FIL			file;
    FIL* mp3File;
#else
    FILE* mp3File;                                        // MP3 file object
#endif
    uint32_t      f_size;                                       // file size
    uint32_t      bytes_remaining;                                 // Encoded MP3 bytes remaining to be processed by either offset or decodeMP3
    bool          file_opened;                                     // true if there is a loaded file
    uint16_t      last_frame_length;                                // Last frame length

    // MP3-encoded buffer
    uint8_t       encoded_frame_buffer[MP3_FRAME_BUFFER_BYTES];         // buffer for MP3-encoded frames
    uint32_t      top_index;                                            // current position in frame buffer (points to top_index)
    uint32_t      bottom_index;                                         // current position at info end in frame buffer

    // ID3 tag
    bool                  has_ID3_Tag;                              // True if the file has valid ID3 tag
    mp3_decoder_tag_data_t ID3_data;                                // Parsed data from ID3 tag


}mp3_decoder_context_t;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

 //File management functions
static bool open_file(char * file_name);
static void close_file(void);
static void fileSeek(size_t pos);
uint32_t getFileSize(void);
void fileRewind(void);
uint16_t readFile(void* buf, uint16_t cnt);

//read ID3
void readID3Tag(void);

//data management
static void copyDataAndMovePointer(void);
void copyFrameInfo(mp3_decoder_frame_data_t* mp3_data, MP3FrameInfo* helix_data);
void resetContextData(void);
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static mp3_decoder_context_t context_data;
#ifdef __arm__
static FRESULT fr;
#endif // __arm__


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void  MP3DecoderInit(void) {
    resetContextData();
    context_data.Decoder = MP3InitDecoder();//Helix decoder init
    
#ifdef DEBUG
    printf("MP3 decoder initialized");
#endif // DEBUG

}

bool  MP3LoadFile(const char* file_name) {
    bool res=false;
    if (context_data.file_opened == true) {//if there was a opened file, i must close it before opening a new one
        resetContextData();
        close_file();
    }
    if (open_file(file_name)) {
        context_data.file_opened = true;
        context_data.f_size = getFileSize();
        context_data.bytes_remaining = context_data.f_size;
        readID3Tag();
#ifdef DEBUG
        printf("File opened successfully!\nFile size is %d bytes\n", context_data.f_size);
#endif
        res = true;
    }
    return res;

}

bool MP3GetTagData(mp3_decoder_tag_data_t* data) {
    bool res = false;
    if (context_data.has_ID3_Tag)
    {
        strcpy(data->album, context_data.ID3_data.album);
        strcpy(data->artist, context_data.ID3_data.artist);
        strcpy(data->title, context_data.ID3_data.title);
        strcpy(data->trackNum, context_data.ID3_data.trackNum);
        strcpy(data->year, context_data.ID3_data.year);
        res = true;
    }
    return res;
}

bool MP3GetLastFrameData(mp3_decoder_frame_data_t* data) {
    bool ret = false;
    if (context_data.bytes_remaining < context_data.f_size)
    {
        copyFrameInfo(data, &context_data.last_frame_info);
        ret = true;
    }
    return ret;
}

bool MP3GetNextFrameData(mp3_decoder_frame_data_t* data) {
    printf("\r\nNOT IMPLEMENTED YET HET NEXT FRAME DATA!!\r\n");
    return true;
}

mp3_decoder_result_t MP3GetDecodedFrame(short* outBuffer, uint16_t bufferSize, uint16_t* samples_decoded, uint8_t depth) {
    mp3_decoder_result_t ret = MP3DECODER_NO_ERROR;    // Return value of the function

#ifdef DEBUG
    printf("Entered decoding. File has %d bytes to decode\n", context_data.f_size);
    printf("Buffer has %d bytes to decode\n", context_data.bottom_index - context_data.top_index);
#endif

    if (depth < MAX_DEPTH)
    {
        if (!context_data.file_opened)
        {
            ret = MP3DECODER_NO_FILE;
#ifdef DEBUG
            printf("There is no opened file\n");
#endif
        }
        else if (context_data.bytes_remaining) // check if there is remaining info to be decoded
        {
#ifdef DEBUG
            printf("Current pointers are Head = %d - Bottom = %d\n", context_data.top_index, context_data.bottom_index);
#endif

            // scroll encoded info up in array if necessary (TESTED-WORKING)
            if ((context_data.top_index > 0) && ((context_data.bottom_index - context_data.top_index) > 0) && (context_data.bottom_index - context_data.top_index < MP3_FRAME_BUFFER_BYTES))
            {
                //memcpy(context_data.mp3FrameBuffer , context_data.mp3FrameBuffer + context_data.top_index, context_data.bottom_index - context_data.top_index);
                memmove(context_data.encoded_frame_buffer, context_data.encoded_frame_buffer + context_data.top_index, context_data.bottom_index - context_data.top_index);
                context_data.bottom_index = context_data.bottom_index - context_data.top_index;
                context_data.top_index = 0;

#ifdef DEBUG
                printf("Copied %d bytes from %d to %d\n", (context_data.bottom_index - context_data.top_index), context_data.top_index, 0);
#endif
            }
            else if (context_data.bottom_index == context_data.top_index)
            {
                // If arrived here, there is nothing else to do
#ifdef DEBUG
                printf("Empty buffer.\n");
#endif

            }
            else if (context_data.bottom_index == MP3_DECODED_BUFFER_SIZE)
            {
#ifdef DEBUG
                printf("Full buffer.\n");
#endif
            }

            // Read encoded data from file
            copyDataAndMovePointer();

            // seek mp3 header beginning 
            int offset = MP3FindSyncWord(context_data.encoded_frame_buffer + context_data.top_index, context_data.bottom_index);

            if (offset >= 0)
            {
                //! check errors in searching for sync words (there shouldnt be)
                context_data.top_index += offset; // updating top_index pointer
                context_data.bytes_remaining -= offset;  // subtract garbage info to file size

#ifdef DEBUG
                printf("Sync word found @ %d offset\n", offset);
#endif
            }

            //check samples in next frame (to avoid segmentation fault)
            MP3FrameInfo nextFrameInfo;
            int err = MP3GetNextFrameInfo(context_data.Decoder, &nextFrameInfo, context_data.encoded_frame_buffer + context_data.top_index);
            if (err == 0)
            {
#ifdef DEBUG
                printf("Frame to decode has %d samples\n", nextFrameInfo.outputSamps);
#endif
                if (nextFrameInfo.outputSamps > bufferSize)
                {
#ifdef DEBUG
                    printf("Out buffer isnt big enough to hold samples.\n");
#endif
                    return MP3DECODER_BUFFER_OVERFLOW;
                }
            }

            // with array organized, lets decode a frame
            uint8_t* decPointer = context_data.encoded_frame_buffer + context_data.top_index;
            int bytesLeft = context_data.bottom_index - context_data.top_index;
            int res = MP3Decode(context_data.Decoder, &decPointer, &(bytesLeft), outBuffer, MP3DECODER_MODE_NORMAL); //! autodecrements fileSize with bytes decoded. updated inbuf pointer, updated bytesLeft

            if (res == ERR_MP3_NONE) // if decoding successful
            {
                uint16_t decodedBytes = context_data.bottom_index - context_data.top_index - bytesLeft;
                context_data.last_frame_length = decodedBytes;

#ifdef DEBUG
                printf("Frame decoded!. MP3 frame size was %d bytes\n", decodedBytes);
#endif

                // update header pointer and file size
                context_data.top_index += decodedBytes;
                context_data.bytes_remaining -= decodedBytes;

                // update last frame decoded info
                MP3GetLastFrameInfo(context_data.Decoder, &(context_data.last_frame_info));

                // update samples decoded
                *samples_decoded = context_data.last_frame_info.outputSamps;

                // return success code
                ret = MP3DECODER_NO_ERROR;
            }
            else if (res == ERR_MP3_INDATA_UNDERFLOW || res == ERR_MP3_MAINDATA_UNDERFLOW)
            {
                if (context_data.bytes_remaining == 0)
                {
#ifdef DEBUG
                    printf("[Error] Buffer underflow and file empty\n");
#endif

                    return MP3DECODER_FILE_END;
                }
#ifdef DEBUG
                printf("Underflow error (code %d)\n", res);
#endif

                // If there weren't enough bytes on the buffer, try again
                return MP3GetDecodedFrame(outBuffer, bufferSize, samples_decoded, depth + 1); //! H-quearlo
            }
            else
            {
                if (context_data.bytes_remaining <= context_data.last_frame_length)
                {
#ifdef DEBUG
                    printf("Dropped frame\n");
#endif
                    return MP3DECODER_FILE_END;
                }
                else
                {
                    context_data.top_index++;
                    context_data.bytes_remaining--;
#ifdef DEBUG
                    printf("Error: %d\n", res);
#endif

                    // If invalid header, try with next frame
                    return MP3GetDecodedFrame(outBuffer, bufferSize, samples_decoded, depth + 1); //! H-quearlo
                }
            }
        }
        else
        {
            ret = MP3DECODER_FILE_END;
        }
    }
    else
    {
        ret = MP3DECODER_ERROR;
    }
    return ret;

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

//File management functions

static bool open_file(char* file_name) {
    bool ret = false;
#ifdef __arm__
    fr = f_open(&context_data.file, file_name, FA_READ);
    if (fr == FR_OK)
    {
        context_data.mp3File = &(context_data.file);
        ret = true;
    }
#else
    context_data.mp3File = fopen(file_name, "rb");
    ret = (context_data.mp3File != NULL);
#endif
    return ret;
}

static void close_file(void) {
#ifdef __arm__
    f_close(context_data.mp3File);
#else
    fclose(context_data.mp3File);
#endif // __arm__
}


static void fileSeek(size_t pos) {
#ifdef __arm__
    f_lseek(context_data.mp3File, pos);
#else
    fseek(context_data.mp3File, pos, SEEK_SET);
#endif
}

uint32_t getFileSize(void) {
    uint32_t ret=0;
    if (context_data.file_opened)
    {
#ifdef __arm__
        ret = f_size(context_data.mp3File);
#else
        fseek(context_data.mp3File, 0L, SEEK_END);
        ret = ftell(context_data.mp3File);
        fileRewind();
        fseek(context_data.mp3File, 0, SEEK_SET);
#endif
    }
    return ret;
}

void fileRewind(void){
#ifdef __arm__
    f_rewind(context_data.mp3File);
#else
    rewind(context_data.mp3File);
#endif
}

uint16_t readFile(void* buf, uint16_t cnt) {
    uint16_t ret = 0;
    uint16_t read;
    if (context_data.file_opened)
    {
#ifdef __arm__
        fr = f_read(context_data.mp3File, ((uint8_t*)buf) + ret, cnt, &read);
        if (fr == FR_OK)
        {
            ret = read;
        }
#else
        ret = fread(buf, 1, cnt, context_data.mp3File);
#endif
    }
    return ret;
}





//ID3
void readID3Tag(void)
{

    if (has_ID3_tag(context_data.mp3File))
    {
        context_data.has_ID3_Tag = true;

        if (!read_ID3_info(TITLE_ID3, context_data.ID3_data.title, ID3_MAX_FIELD_SIZE, context_data.mp3File))
            strcpy(context_data.ID3_data.title, DEFAULT_ID3_FIELD);

        if (!read_ID3_info(ALBUM_ID3, context_data.ID3_data.album, ID3_MAX_FIELD_SIZE, context_data.mp3File))
            strcpy(context_data.ID3_data.album, DEFAULT_ID3_FIELD);

        if (!read_ID3_info(ARTIST_ID3, context_data.ID3_data.artist, ID3_MAX_FIELD_SIZE, context_data.mp3File))
            strcpy(context_data.ID3_data.artist, DEFAULT_ID3_FIELD);

        if (!read_ID3_info(YEAR_ID3, context_data.ID3_data.year, 10, context_data.mp3File))
            strcpy(context_data.ID3_data.year, DEFAULT_ID3_FIELD);

        if (!read_ID3_info(TRACK_NUM_ID3, context_data.ID3_data.trackNum, 10, context_data.mp3File))
            strcpy(context_data.ID3_data.trackNum, DEFAULT_ID3_FIELD);


        unsigned int tagSize = get_ID3_size(context_data.mp3File);

#ifdef DEBUG
        printf("ID3 Track found.\n");
        printf("ID3 Tag is %d bytes long\n", tagSize);
#endif    
        fileSeek(tagSize);
        context_data.bytes_remaining -= tagSize;
    }
    else
    {
        fileRewind();
    }
}

void copyDataAndMovePointer() {
    uint16_t bytes_read;

    // Fill buffer with info in mp3 file
    uint8_t* dst = context_data.encoded_frame_buffer + context_data.bottom_index;

    bytes_read = readFile(dst, (MP3_FRAME_BUFFER_BYTES - context_data.bottom_index));
    // Update bottom_index pointer
    context_data.bottom_index += bytes_read;

#ifdef DEBUG
    if (bytes_read == 0)
    {
        printf("File was read completely.\n");
    }
    printf("[?] Read %d bytes from file. Head = %d - Bottom = %d\n", bytes_read, context_data.top_index, context_data.bottom_index);
#endif
}

void copyFrameInfo(mp3_decoder_frame_data_t* mp3_data, MP3FrameInfo* helix_data)
{
    mp3_data->bitRate = helix_data->bitrate;
    mp3_data->binitsPerSample = helix_data->bitsPerSample;
    mp3_data->channelCount = helix_data->nChans;
    mp3_data->sampleRate = helix_data->samprate;
    mp3_data->sampleCount = helix_data->outputSamps;
}
void resetContextData(void) {
    context_data.file_opened = false;
    context_data.has_ID3_Tag = false;
    context_data.bottom_index = 0;
    context_data.top_index = 0;
    context_data.bytes_remaining = 0;
    context_data.f_size = 0;
}