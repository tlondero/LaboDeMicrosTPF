/***************************************************************************/ /**
  @file     FileName.h
  @brief	///
  @author   Guido Lambertucci
 ******************************************************************************/
#ifndef PACKETS_H_
#define PACKETS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
/*
 * PAQUETES
 *
 * EL TERMINADOR SIEMPRE ES \r\n
 *
 * '00' (MENU)-> opciones:
 * 							"File System Explorer Menu"					--> listo
 * 							"Equalizer Menu"							--> listo
 * 							"Spectrogram on" or "Spectrogram off"		--> listo
 *
 * 	'01' (ID3 Name) "name"												--> listo
 * 	'02' (ID3 artist) "artist"											--> listo
 * 	'03' (ID3 album) "album"											--> listo
 * 	'04' (ID3 track) "track"											--> listo
 * 	'05' (ID3 year) "year"												--> listo
 *
 * 	'06' dir path
 *
 * 	'07' SD I/O-> opciones:												--> listo
 * 								'I' or 'O'
 *
 * 	'08' Playing/stop -> opciones:										--> listo
 * 								'P' or 'S'
 * 	'09' Volume -> opciones:											--> listo
 * 							volume char. 0 ~ 30
 *
 * 	'10' (KIN)-> opciones: 												--> listo
 * 							'P' pr 'A'

 *	'11' EQUALIZADOR -> opciones: 										--> listo
 *									"ROCK"
 *									"JAZZ"
 *									"CLASICO"
 *
 * */

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief  --
 * @param  --
 * @return --
*/

#endif /* PACKETS_H_ */
