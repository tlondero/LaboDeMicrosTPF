#include "encoder.h"

#include "board.h"
#include "core_cm4.h"
#include <stdbool.h>
#include "fsl_port.h"
#include "fsl_sdmmc_osa.h"
#include "fsl_pit.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_ENCODERS 5
#define NO_MORE_ENCODERS 255

#define HIGH 1
#define LOW 0

#define LEN(array) sizeof(array) / sizeof(array[0])
#define PORT_ENCODER_A PORTB
#define PIN_ENCODER_A	18

#define PORT_ENCODER_B PORTB
#define PIN_ENCODER_B	19
#define GPIO_ENCODER GPIOB
/*******************************************************************************
 * STRUCTS AND TYPEDEFS
 ******************************************************************************/
typedef struct
{
	uint8_t pin_A;
	uint8_t pin_B;
	bool event_flag;
	event_enc_t event_queue[20];
//	tim_id_t timer_id;
	uint8_t in_pointer;
	uint8_t out_pointer;

} encoder_state_t;

/*******************************************************************************
 * STATIC VARIABLES
 ******************************************************************************/
static encoder_state_t encoders[MAX_ENCODERS];
static uint16_t enconders_cant = 0;
//static tim_id_t encoder_timer_id;
static bool busy=false;

/*******************************************************************************
 * FUNCTION DECLARATIONS WITH LOCAL SCOPE
 ******************************************************************************/

/**
 * @brief Adds a new a event to the queue
 * @param encoder_id unique encoder identifier
 * @param event_t RIGHT_TURN LEFT_TURN
 */
void EncoderAddNewEvent(encoder_id id, event_enc_t ev);
/**
 * @brief Updates the event queue if a new event has been detected
 */
void EncoderUpdate(void);

/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/

void EncoderInit(encoder_id enc_id)
{
	static bool warm_up_rdy = false;
	if (!warm_up_rdy)
	{
		warm_up_rdy = true;
//		timerInit();
//		encoder_timer_id = timerGetId();
//		encoders[enc_id].timer_id = encoder_timer_id;
//		timerStart(encoder_timer_id, (uint32_t)TIMER_MS2TICKS(100), TIM_MODE_SINGLESHOT, NULL); //Previene rebotes

#if DEBUGGIN_MODE_ENCODER
		gpioMode(DEBUG_PIN, OUTPUT);
		gpioWrite(DEBUG_PIN, LOW);
#endif
	}
}

//Adds new encoder
encoder_id EncoderRegister()
{

	encoder_id id;
	id = enconders_cant++;
	EncoderInit(id);

	encoders[id].event_flag = false;

	//Init circular buffer
	encoders[id].in_pointer = 0;
	encoders[id].out_pointer = 0;
	port_pin_config_t conf;
	conf.driveStrength= kPORT_LowDriveStrength;
	conf.lockRegister = kPORT_UnlockRegister;
	conf.mux = kPORT_MuxAsGpio;
	conf.openDrainEnable= kPORT_OpenDrainDisable;
	conf.passiveFilterEnable=kPORT_PassiveFilterDisable;
	conf.pullSelect=kPORT_PullUp;
	conf.slewRate = kPORT_FastSlewRate;

	PORT_SetPinConfig(PORT_ENCODER_A, PIN_ENCODER_A, &conf); // pin A del encoder configured

	PORT_SetPinConfig(PORT_ENCODER_B, PIN_ENCODER_B, &conf);// pin B del encoder configured

//	gpioMode(encoders[id].pin_B, INPUT_PULLUP);

//	gpioIRQ(encoders[id].pin_B, GPIO_IRQ_MODE_FALLING_EDGE, EncoderUpdate);

	PORT_SetPinInterruptConfig(PORT_ENCODER_B, PIN_ENCODER_B, kPORT_InterruptFallingEdge);

	NVIC_EnableIRQ(PORTB_IRQn);


	PIT_SetTimerPeriod(PIT, kPIT_Chnl_2,
					USEC_TO_COUNT(50000U, CLOCK_GetFreq(kCLOCK_BusClk)));
	PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
	EnableIRQ(PIT2_IRQn);
	return id;
}

//Update encoders
void EncoderUpdate(void)
{

	encoder_id id = 0;

	if(!busy){
	PIT_StartTimer(PIT, kPIT_Chnl_2);
	busy=true;
	for (id = 0; id < enconders_cant; id++){
			if (GPIO_PinRead(GPIO_ENCODER, PIN_ENCODER_A) == HIGH)
			{
				EncoderAddNewEvent(id, RIGHT_TURN);
			}
			else if (GPIO_PinRead(GPIO_ENCODER, PIN_ENCODER_A) == LOW){
				EncoderAddNewEvent(id, LEFT_TURN);
			}
	}
	}

}
//Reports if a new event is available
bool EncoderEventAVB(encoder_id id)
{
	return encoders[id].event_flag;
}
//Circular event buffer
//Adds a new a event to the queue
void EncoderAddNewEvent(encoder_id id, event_enc_t ev)
{
	if (encoders[id].in_pointer != (encoders[id].out_pointer + LEN(encoders[id].event_queue)))
	{
		encoders[id].event_queue[encoders[id].in_pointer] = ev;
		encoders[id].in_pointer = (++encoders[id].in_pointer) % (LEN(encoders[id].event_queue));
		encoders[id].event_flag = EVENT_AVB;
	}
}
//Pops the last event in the queue
event_enc_t EncoderPopEvent(encoder_id id)
{
	//Is the buffer non-empty?
	if (encoders[id].out_pointer != encoders[id].in_pointer)
	{
		event_enc_t ev;
		ev = encoders[id].event_queue[encoders[id].out_pointer];
		encoders[id].out_pointer = (++encoders[id].out_pointer) % (LEN(encoders[id].event_queue));
		if (encoders[id].out_pointer == encoders[id].in_pointer)
		{
			encoders[id].event_flag = EVENT_NOT_AVB;
		}
		return ev;
	}
	else
		return EVENT_NOT_AVB;
}

void PORTB_IRQHandler(void)   //encoder
{
	 if (((1U << PIN_ENCODER_B) & PORT_GetPinsInterruptFlags(PORTB))){
	    PORT_ClearPinsInterruptFlags(PORTB, (1U << PIN_ENCODER_B));

	    EncoderUpdate();

	 }
}



void PIT2_IRQHandler(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);

	busy=false;
	PIT_StopTimer(PIT, kPIT_Chnl_2);
	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();

}
