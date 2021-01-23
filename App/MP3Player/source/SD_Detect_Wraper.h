
/***************************************************************************/ /**
  @file     SD_Detect_Wraper.h
  @brief
  @author   MAGT
 ******************************************************************************/
#ifndef SD_DETECT_WRAPER_H_
#define SD_DETECT_WRAPER_H_
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsl_common.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void (*cback) (void);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the PIT module and hosts the SD, when a sd is inserted will mount the fileSystem in it.
 * @param
 * 		@inserted_callback -- function pointer to the callback when a sd card is inserted.
 * 		@extracted_callback -- function pointer to the callback when a sd card is extracted.
 *
 * @return true if it was succesfull the initialization.
*/
bool SDWraperInit(cback inserted_callback, cback extracted_callback);


/**
 * @brief  getter for the SD status
 * @param --
 * @return true if it is inserted, false if it isnt.
*/
bool getSDInserted(void);


/**
 * @brief getter that announces if the SD was just introduced
 * @param --
 * @return True if it was inserted.
*/
bool getJustIn(void);


/**
 * @brief getter that announces if the SD was just extracted
 * @param --
 * @return True if it was extracted
*/
bool getJustOut(void);

/**
 * @brief getter for the error status
 * @param --
 * @return see FRESULT typedef for the errors.
*/
FRESULT getErrorStatus(void);


#endif /* SD_DETECT_WRAPER_H_ */
