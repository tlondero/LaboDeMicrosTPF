/***************************************************************************/ /**
  @file     FS_explorer.h
  @brief	API  simple and usefull to explore directories
  	  	  	in a FAT volume.
  @author   Guido Lambertucci
 ******************************************************************************/
#ifndef FS_EXPLORER_H_
#define FS_EXPLORER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FSEXP_ROOT_DIR ("/")
#define NAMES_BUFFER_SIZE 255
#define NAME_LIST_SIZE 20
#define MAX_PATH_LENGHT 1000


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void (*cback)(void);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  exploreFS
 * @param
 * 		@path: path to the directory start, usually "/"
 * @return  returns a char * to the first directory/file name in the path given.
*/
char * FSEXP_exploreFS(char * path);

/**
 * @brief  returns the mp3 path for the decoder
 * @param
 * 	@path:
 * @return  returns a char * to the mp3path.
*/
char * FSEXP_getMP3Path(void);

/**
 * @brief  getNext: gives the next name in the directory, if the directory is finished will return a NULL ptr.
 * @param  --
 * @return --
*/
char* FSEXP_getNext();


/**
 * @brief  getPrev: gives the previous name in the directory, if the directory is finished will return a NULL ptr.
 * @param  --
 * @return --
*/
char* FSEXP_getPrev();


/**
 * @brief  openSelected: opens the selected directory/file, if its a  directory will open it and return
 * a char ptr to the first filename of the dir, if its a file will call the callback provided by user with the method addCallbackForFile
 * @param  --
 * @return --
*/
char *  FSEXP_openSelected();


/**
 * @brief  goBackDir, if you are in a nested directory this function will go back a directory, if its on the first layer will return NULL
 * @param  --
 * @return --
*/
char *  FSEXP_goBackDir();


/**
 * @brief  getPath: returns the accumulated path char ptr
 * @param  --
 * @return --
*/
char * FSEXP_getPath(void);


/**
 * @brief  addCallbackForFile: provides a callback to be called when a file is selected to open
 * @param  --
 * @return --
*/
void FSEXP_addCallbackForFile(cback cb);


/**

 * @brief  FSEXP_getFilename: returns name of pointed file
 * @param  --
 * @return pointed file
*/
char * FSEXP_getFilename(void);


/*
 * @brief  FSEXP_closeDir: closes directory
 * @param  --
 * @return --
*/
void FSEXP_closeDir(void);


/*
 * @brief  FSEXP_isdir: indicates if is a dir
 * @param  --
 * @return --
*/
bool FSEXP_isdir(void);


void copyFname(char *destiny, char *source);

#endif /* FS_EXPLORER_H_ */
