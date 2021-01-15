
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


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the PIT module and hosts the SD, when a sd is inserted will mount the fileSystem in it.
 * @param --
 * @return true if it was succesfull the initialization.
*/
bool SDWraperInit(void);


/**
 * @brief  getter for the SD status
 * @param --
 * @return true if it is inserted, false if it isnt.
*/
bool getSDInserted(void);


/**
 * @brief getter that announces if the SD was just introduced
 * @param --
 * @return self explanitory.
*/
bool getJustIn(void);


/**
 * @brief getter that announces if the SD was just extracted
 * @param --
 * @return self explanitory
*/
bool getJustOut(void);



#endif /* SD_DETECT_WRAPER_H_ */
