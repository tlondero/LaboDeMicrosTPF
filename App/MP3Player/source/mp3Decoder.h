
#ifndef _MP3DECODER_H_
#define _MP3DECODER_H_

 /*******************************************************************************
  * INCLUDE HEADER FILES
  ******************************************************************************/

#include  <stdbool.h>
#include  <stdint.h>

  /*******************************************************************************
   * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
   ******************************************************************************/

#define MP3_DECODED_BUFFER_SIZE (5000)                                     // maximum frame size if max bitrate is used (in samples)
#define ID3_MAX_FIELD_SIZE      50

   /*******************************************************************************
    * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
    ******************************************************************************/

typedef enum
{
    MP3DECODER_NO_ERROR,
    MP3DECODER_FILE_END,
    MP3DECODER_NO_FILE,
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
    char title[ID3_MAX_FIELD_SIZE];
    char artist[ID3_MAX_FIELD_SIZE];
    char album[ID3_MAX_FIELD_SIZE];
    char trackNum[10];
    char year[10];

} mp3_decoder_tag_data_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief  MP3DecoderInit: inits the MP3  decoder from Helix library and resets the context data
 * @param  --
 * @return --
*/

void  MP3DecoderInit(void);

/**
 * @brief  MP3LoadFile: Loads the mp3 file , provide a char* filename, for this the filesystem must be mounted
 * @param  file_name a pointer to the file_name
 * @return bool: returns true if it was able to open the file
*/
bool  MP3LoadFile(const char* file_name);

/**
 * @brief  MP3GetTagData: get the ID3 data read in the MP3 Load file
 * @param  data* data structure for the ID3 data
 * @return returns true if it has ID3 data
*/
bool MP3GetTagData(mp3_decoder_tag_data_t* data);

/**
 * @brief  MP3GetLastFrameData Getter for the last frame data
 * @param  data* data structure for the frame
 * @return  returns true if it was succesfull
*/
bool MP3GetLastFrameData(mp3_decoder_frame_data_t* data);



/**
 * @brief  MP3GetDecodedFrame
 * @param
 * 			@outBuffer buffer to be filled
 * 			@bufferSize outBuffer size
 * 			@samples_decoded pointer to a counter that is modified by the amaount of samples decoded
 * @return mp3_decoder_result_t
*/
mp3_decoder_result_t MP3GetDecodedFrame(short* outBuffer, uint16_t bufferSize, uint16_t* samples_decoded);


#endif

