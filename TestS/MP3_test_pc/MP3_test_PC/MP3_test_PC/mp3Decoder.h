
#ifndef _MP3DECODER_H_
#define _MP3DECODER_H_

 /*******************************************************************************
  * INCLUDE HEADER FILES
  ******************************************************************************/
#define DEBUG
#include  <stdbool.h>
#include  <stdint.h>

  /*******************************************************************************
   * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
   ******************************************************************************/

#define MP3_DECODED_BUFFER_SIZE (4*1152)                                     // maximum frame size if max bitrate is used (in samples)
#define ID3_MAX_FIELD_SIZE      50

   /*******************************************************************************
    * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
    ******************************************************************************/

typedef enum
{
    MP3DECODER_NO_ERROR,
    MP3DECODER_ERROR,
    MP3DECODER_FILE_END,
    MP3DECODER_NO_FILE,
    MP3DECODER_BUFFER_OVERFLOW
} mp3_decoder_result_t;

typedef struct
{
    uint16_t    bitRate;
    uint8_t     channelCount;
    uint16_t	sampleRate;
    uint16_t    binitsPerSample;
    uint16_t    sampleCount;
} mp3_decoder_frame_data_t;

typedef struct
{
    uint8_t title[ID3_MAX_FIELD_SIZE];
    uint8_t artist[ID3_MAX_FIELD_SIZE];
    uint8_t album[ID3_MAX_FIELD_SIZE];
    uint8_t trackNum[10];
    uint8_t year[10];

} mp3_decoder_tag_data_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void  MP3DecoderInit(void);
bool  MP3LoadFile(const char* file_name);
bool MP3GetTagData(mp3_decoder_tag_data_t* data);
bool MP3GetLastFrameData(mp3_decoder_frame_data_t* data);
bool MP3GetNextFrameData(mp3_decoder_frame_data_t* data);
mp3_decoder_result_t MP3GetDecodedFrame(short* outBuffer, uint16_t bufferSize, uint16_t* samples_decoded, uint8_t depth);

#endif