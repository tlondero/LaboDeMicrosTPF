/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "equaliser.h"
#include "math_helper.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SNR_THRESHOLD_F32    	130.0f

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// static void privateFunction(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

extern float32_t testInput_f32_1kHz_15kHz[1024];
extern float32_t refOutput[1024];

static float32_t testOutput[1024];
float32_t  snr;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
int main(void){
	appInit();
	while(1){
		appRun();
	}
	return 0;
}
/* Called once at the beginning of the program */
void appInit (void)
{
    eqInit(1024);
//    float32_t filterGains[8] = {3.0f, 0.1f, 6.0f, 0.5f, 2.0f, 0.0f, 0.9f, 5.0f};
//	eqSetFilterGains(filterGains);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	arm_status status;

	eqFilterFrame(&testInput_f32_1kHz_15kHz[0], &testOutput[0]);

	/* ----------------------------------------------------------------------
	** Compare the generated output against the reference output computed
	** in MATLAB.
	** ------------------------------------------------------------------- */

	snr = arm_snr_f32(&refOutput[0], &testOutput[0], 1024);

	if (snr < SNR_THRESHOLD_F32)
	{
	  status = ARM_MATH_TEST_FAILURE;
	}
	else
	{
	  status = ARM_MATH_SUCCESS;
	}

	/* ----------------------------------------------------------------------
	** Loop here if the signal does not match the reference output.
	** ------------------------------------------------------------------- */

	if ( status != ARM_MATH_SUCCESS)
	{
	  while (1);
	}

	while (1);                             /* main function does not return */
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
