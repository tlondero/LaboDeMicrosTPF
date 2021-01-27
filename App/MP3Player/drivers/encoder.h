/***************************************************************************/ /**
  @file     encoder.h
  @brief    Rotary encoder driver
  @author   MAGT
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "fsl_gpio.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef uint8_t event_t;
typedef uint8_t encoder_id;

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define EVENT_AVB 1     //New event available
#define EVENT_NOT_AVB 0 //Nothing new to retrieve

//EVENTS
#define LEFT_TURN 0
#define RIGHT_TURN 1

/**
 * @brief Registers a new encoder
 * @param pin_A Clock
 * @param pin_B Data
 */
encoder_id EncoderRegister(uint8_t pin_A, uint8_t pin_B);
/**
 * @brief Reports if there is an event available
 * @param encoder_id unique encoder identifier
 * @returns true if a new event is available, false otherwise
 */
bool EncoderEventAVB(encoder_id id);

/**
 * @brief Pops the last event in the queue
 * @param encoder_id unique encoder identifier
 * @returns LEFT_TURN or RIGHT_TURN  if events still available, EVENT_NOT_AVB otherwise
 */
event_t EncoderPopEvent(encoder_id id);
