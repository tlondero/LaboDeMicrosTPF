
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
#include "ff.h"
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
bool SDWRAPPER_Init(cback inserted_callback, cback extracted_callback);

/**
 * @brief Sets the interrupt for the SD card
 *
*/
void SDWRAPPER_SetInterruptEnable(bool enable);

/**
 * @brief Sets the power of the SD Card.
 * @param
 * 		@enable -- whether to turn on or off the power
*/
void SDWRAPPER_SetCardPower(bool enable);

/**
 * @brief  getter for the SD status
 * @param --
 * @return true if it is inserted, false if it isnt.
*/
bool SDWRAPPER_getSDInserted(void);


/**
 * @brief getter that announces if the SD was just introduced
 * @param --
 * @return True if it was inserted.
*/
bool SDWRAPPER_getJustIn(void);


/**
 * @brief getter that announces if the SD was just extracted
 * @param --
 * @return True if it was extracted
*/
bool SDWRAPPER_getJustOut(void);

/**
 * @brief getter for the error status
 * @param --
 * @return see FRESULT typedef for the errors.
*/
FRESULT SDWRAPPER_getErrorStatus(void);

/**
 * @brief sets the power level for the SD card
 * @param enable -- whether to power it on or off
*/
void SDWRAPPER_SetCardPower(bool enable);

/**
 * @brief clears the just inserted flag, this will make so that the next call
 * 			to getjustin returns false as if it was polled before
*/
void SDWRAPPER_ClearInsertedFlag(void);

/**
 * @brief set the just inserted flag, this will make so that the next call
 * 			to getjustin returns true as if the card was just inserted
*/
void SDWRAPPER_SetInsertedFlag(void);

/**
 * @brief check whether filesystem was mounted already
 *
*/
bool SDWRAPPER_GetMounted(void);

#endif /* SD_DETECT_WRAPER_H_ */
