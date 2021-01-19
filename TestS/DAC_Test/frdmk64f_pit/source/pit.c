/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_pit.h"

#include "DAC_Wrapper.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_PIT_BASEADDR PIT
#define DEMO_PIT_CHANNEL  kPIT_Chnl_0
#define PIT_LED_HANDLER   PIT0_IRQHandler
#define PIT_IRQ_ID        PIT0_IRQn
/* Get source clock for PIT driver */
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define LED_INIT()       LED_RED_INIT(LOGIC_LED_ON)
#define LED_TOGGLE()     LED_RED_TOGGLE()

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

typedef enum _mp3_sample_rate {
	kMP3_8000Hz,
	kMP3_11025Hz,
	kMP3_12000Hz,
	kMP3_16000Hz,
	kMP3_22050Hz,
	kMP3_24000Hz,
	kMP3_32000Hz,
	kMP3_44100Hz,
	kMP3_48000Hz
} mp3_sample_rate_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile bool pitIsrFlag = false;

uint16_t sen1k[DAC_USED_BUFFER_SIZE] = { 307U, 309U, 310U, 312U, 314U, 316U,
		318U, 320U, 322U, 324U, 325U, 327U, 329U, 331U, 333U, 335U, 337U, 339U,
		341U, 342U, 344U, 346U, 348U, 350U, 352U, 354U, 355U, 357U, 359U, 361U,
		363U, 365U, 367U, 368U, 370U, 372U, 374U, 376U, 378U, 379U, 381U, 383U,
		385U, 387U, 389U, 390U, 392U, 394U, 396U, 398U, 399U, 401U, 403U, 405U,
		407U, 408U, 410U, 412U, 414U, 415U, 417U, 419U, 421U, 423U, 424U, 426U,
		428U, 429U, 431U, 433U, 435U, 436U, 438U, 440U, 441U, 443U, 445U, 447U,
		448U, 450U, 452U, 453U, 455U, 456U, 458U, 460U, 461U, 463U, 465U, 466U,
		468U, 469U, 471U, 473U, 474U, 476U, 477U, 479U, 481U, 482U, 484U, 485U,
		487U, 488U, 490U, 491U, 493U, 494U, 496U, 497U, 499U, 500U, 502U, 503U,
		505U, 506U, 507U, 509U, 510U, 512U, 513U, 514U, 516U, 517U, 519U, 520U,
		521U, 523U, 524U, 525U, 527U, 528U, 529U, 531U, 532U, 533U, 534U, 536U,
		537U, 538U, 539U, 541U, 542U, 543U, 544U, 545U, 547U, 548U, 549U, 550U,
		551U, 552U, 553U, 555U, 556U, 557U, 558U, 559U, 560U, 561U, 562U, 563U,
		564U, 565U, 566U, 567U, 568U, 569U, 570U, 571U, 572U, 573U, 574U, 575U,
		576U, 577U, 578U, 579U, 579U, 580U, 581U, 582U, 583U, 584U, 584U, 585U,
		586U, 587U, 588U, 588U, 589U, 590U, 591U, 591U, 592U, 593U, 593U, 594U,
		595U, 595U, 596U, 597U, 597U, 598U, 598U, 599U, 600U, 600U, 601U, 601U,
		602U, 602U, 603U, 603U, 604U, 604U, 605U, 605U, 606U, 606U, 606U, 607U,
		607U, 608U, 608U, 608U, 609U, 609U, 609U, 610U, 610U, 610U, 610U, 611U,
		611U, 611U, 611U, 612U, 612U, 612U, 612U, 612U, 613U, 613U, 613U, 613U,
		613U, 613U, 613U, 613U, 614U, 614U, 614U, 614U, 614U, 614U, 614U, 614U,
		614U, 614U, 614U, 614U, 614U, 613U, 613U, 613U, 613U, 613U, 613U, 613U,
		613U, 612U, 612U, 612U, 612U, 612U, 611U, 611U, 611U, 611U, 610U, 610U,
		610U, 609U, 609U, 609U, 608U, 608U, 608U, 607U, 607U, 607U, 606U, 606U,
		605U, 605U, 604U, 604U, 603U, 603U, 602U, 602U, 601U, 601U, 600U, 600U,
		599U, 599U, 598U, 597U, 597U, 596U, 596U, 595U, 594U, 594U, 593U, 592U,
		592U, 591U, 590U, 589U, 589U, 588U, 587U, 586U, 586U, 585U, 584U, 583U,
		582U, 582U, 581U, 580U, 579U, 578U, 577U, 576U, 575U, 574U, 574U, 573U,
		572U, 571U, 570U, 569U, 568U, 567U, 566U, 565U, 564U, 563U, 562U, 561U,
		560U, 558U, 557U, 556U, 555U, 554U, 553U, 552U, 551U, 549U, 548U, 547U,
		546U, 545U, 544U, 542U, 541U, 540U, 539U, 537U, 536U, 535U, 534U, 532U,
		531U, 530U, 529U, 527U, 526U, 525U, 523U, 522U, 521U, 519U, 518U, 517U,
		515U, 514U, 512U, 511U, 510U, 508U, 507U, 505U, 504U, 502U, 501U, 499U,
		498U, 496U, 495U, 493U, 492U, 490U, 489U, 487U, 486U, 484U, 483U, 481U,
		480U, 478U, 477U, 475U, 473U, 472U, 470U, 469U, 467U, 465U, 464U, 462U,
		461U, 459U, 457U, 456U, 454U, 452U, 451U, 449U, 447U, 446U, 444U, 442U,
		441U, 439U, 437U, 435U, 434U, 432U, 430U, 429U, 427U, 425U, 423U, 422U,
		420U, 418U, 416U, 415U, 413U, 411U, 409U, 408U, 406U, 404U, 402U, 400U,
		399U, 397U, 395U, 393U, 391U, 390U, 388U, 386U, 384U, 382U, 380U, 379U,
		377U, 375U, 373U, 371U, 369U, 368U, 366U, 364U, 362U, 360U, 358U, 356U,
		355U, 353U, 351U, 349U, 347U, 345U, 343U, 341U, 340U, 338U, 336U, 334U,
		332U, 330U, 328U, 326U, 325U, 323U, 321U, 319U, 317U, 315U, 313U, 311U,
		309U, 308U, 306U, 304U, 302U, 300U, 298U, 296U, 294U, 292U, 291U, 289U,
		287U, 285U, 283U, 281U, 279U, 277U, 276U, 274U, 272U, 270U, 268U, 266U,
		264U, 262U, 261U, 259U, 257U, 255U, 253U, 251U, 249U, 248U, 246U, 244U,
		242U, 240U, 238U, 237U, 235U, 233U, 231U, 229U, 227U, 226U, 224U, 222U,
		220U, 218U, 216U, 215U, 213U, 211U, 209U, 208U, 206U, 204U, 202U, 200U,
		199U, 197U, 195U, 193U, 192U, 190U, 188U, 186U, 185U, 183U, 181U, 179U,
		178U, 176U, 174U, 173U, 171U, 169U, 168U, 166U, 164U, 163U, 161U, 159U,
		158U, 156U, 154U, 153U, 151U, 149U, 148U, 146U, 145U, 143U, 141U, 140U,
		138U, 137U, 135U, 134U, 132U, 130U, 129U, 127U, 126U, 124U, 123U, 121U,
		120U, 118U, 117U, 115U, 114U, 112U, 111U, 109U, 108U, 107U, 105U, 104U,
		102U, 101U, 100U, 98U, 97U, 95U, 94U, 93U, 91U, 90U, 89U, 87U, 86U, 85U,
		83U, 82U, 81U, 80U, 78U, 77U, 76U, 75U, 73U, 72U, 71U, 70U, 68U, 67U,
		66U, 65U, 64U, 63U, 61U, 60U, 59U, 58U, 57U, 56U, 55U, 54U, 53U, 52U,
		51U, 50U, 49U, 47U, 46U, 45U, 44U, 44U, 43U, 42U, 41U, 40U, 39U, 38U,
		37U, 36U, 35U, 34U, 33U, 33U, 32U, 31U, 30U, 29U, 28U, 28U, 27U, 26U,
		25U, 25U, 24U, 23U, 22U, 22U, 21U, 20U, 20U, 19U, 18U, 18U, 17U, 16U,
		16U, 15U, 15U, 14U, 13U, 13U, 12U, 12U, 11U, 11U, 10U, 10U, 9U, 9U, 8U,
		8U, 8U, 7U, 7U, 6U, 6U, 6U, 5U, 5U, 4U, 4U, 4U, 3U, 3U, 3U, 3U, 2U, 2U,
		2U, 2U, 1U, 1U, 1U, 1U, 1U, 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
		0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 1U,
		1U, 1U, 1U, 2U, 2U, 2U, 2U, 2U, 3U, 3U, 3U, 4U, 4U, 4U, 5U, 5U, 5U, 6U,
		6U, 6U, 7U, 7U, 8U, 8U, 9U, 9U, 10U, 10U, 11U, 11U, 12U, 12U, 13U, 13U,
		14U, 14U, 15U, 15U, 16U, 17U, 17U, 18U, 19U, 19U, 20U, 21U, 21U, 22U,
		23U, 23U, 24U, 25U, 26U, 26U, 27U, 28U, 29U, 30U, 30U, 31U, 32U, 33U,
		34U, 35U, 36U, 37U, 37U, 38U, 39U, 40U, 41U, 42U, 43U, 44U, 45U, 46U,
		47U, 48U, 49U, 50U, 51U, 52U, 53U, 54U, 55U, 56U, 58U, 59U, 60U, 61U,
		62U, 63U, 64U, 66U, 67U, 68U, 69U, 70U, 71U, 73U, 74U, 75U, 76U, 78U,
		79U, 80U, 81U, 83U, 84U, 85U, 87U, 88U, 89U, 91U, 92U, 93U, 95U, 96U,
		97U, 99U, 100U, 102U, 103U, 104U, 106U, 107U, 109U, 110U, 112U, 113U,
		115U, 116U, 118U, 119U, 121U, 122U, 124U, 125U, 127U, 128U, 130U, 131U,
		133U, 134U, 136U, 137U, 139U, 141U, 142U, 144U, 145U, 147U, 149U, 150U,
		152U, 153U, 155U, 157U, 158U, 160U, 162U, 163U, 165U, 167U, 168U, 170U,
		172U, 173U, 175U, 177U, 179U, 180U, 182U, 184U, 186U, 187U, 189U, 191U,
		192U, 194U, 196U, 198U, 200U, 201U, 203U, 205U, 207U, 208U, 210U, 212U,
		214U, 216U, 217U, 219U, 221U, 223U, 225U, 226U, 228U, 230U, 232U, 234U,
		236U, 237U, 239U, 241U, 243U, 245U, 247U, 249U, 250U, 252U, 254U, 256U,
		258U, 260U, 262U, 263U, 265U, 267U, 269U, 271U, 273U, 275U, 276U, 278U,
		280U, 282U, 284U, 286U, 288U, 290U, 292U, 293U, 295U, 297U, 299U, 301U,
		303U, 305U, 307U };

uint16_t sen15k[DAC_USED_BUFFER_SIZE] = { 307U, 335U, 363U, 390U, 417U, 443U,
		468U, 491U, 513U, 533U, 551U, 567U, 581U, 593U, 602U, 608U, 612U, 614U,
		613U, 609U, 602U, 594U, 582U, 569U, 553U, 535U, 515U, 493U, 470U, 446U,
		420U, 393U, 366U, 338U, 309U, 281U, 253U, 226U, 199U, 173U, 148U, 124U,
		102U, 82U, 64U, 47U, 33U, 22U, 12U, 6U, 1U, 0U, 0U, 4U, 10U, 19U, 30U,
		43U, 59U, 76U, 96U, 118U, 141U, 165U, 191U, 217U, 245U, 273U, 301U,
		329U, 357U, 385U, 412U, 438U, 463U, 487U, 509U, 529U, 548U, 564U, 579U,
		591U, 600U, 607U, 612U, 614U, 613U, 610U, 604U, 596U, 585U, 572U, 556U,
		539U, 519U, 498U, 475U, 451U, 425U, 399U, 371U, 343U, 315U, 287U, 259U,
		231U, 204U, 178U, 153U, 129U, 107U, 86U, 67U, 51U, 36U, 24U, 14U, 7U,
		2U, 0U, 0U, 3U, 9U, 17U, 27U, 40U, 55U, 73U, 92U, 113U, 136U, 160U,
		186U, 212U, 239U, 267U, 295U, 324U, 352U, 379U, 407U, 433U, 458U, 482U,
		505U, 525U, 544U, 561U, 576U, 588U, 598U, 606U, 611U, 613U, 613U, 611U,
		605U, 597U, 587U, 574U, 560U, 542U, 523U, 502U, 480U, 456U, 430U, 404U,
		377U, 349U, 321U, 292U, 264U, 237U, 209U, 183U, 158U, 134U, 111U, 90U,
		71U, 54U, 39U, 26U, 16U, 8U, 3U, 0U, 0U, 2U, 7U, 15U, 25U, 37U, 52U,
		69U, 88U, 109U, 131U, 155U, 180U, 207U, 234U, 262U, 290U, 318U, 346U,
		374U, 401U, 428U, 453U, 477U, 500U, 521U, 541U, 558U, 573U, 586U, 597U,
		605U, 610U, 613U, 614U, 611U, 607U, 599U, 589U, 577U, 563U, 546U, 527U,
		507U, 484U, 461U, 435U, 409U, 382U, 355U, 326U, 298U, 270U, 242U, 215U,
		188U, 163U, 138U, 115U, 94U, 75U, 57U, 42U, 28U, 18U, 9U, 3U, 0U, 0U,
		2U, 6U, 13U, 23U, 35U, 49U, 66U, 84U, 104U, 127U, 150U, 175U, 201U,
		228U, 256U, 284U, 312U, 341U, 368U, 396U, 423U, 448U, 473U, 496U, 517U,
		537U, 555U, 570U, 584U, 595U, 603U, 609U, 613U, 614U, 612U, 608U, 601U,
		592U, 580U, 566U, 549U, 531U, 511U, 489U, 465U, 441U, 415U, 388U, 360U,
		332U, 304U, 276U, 248U, 220U, 193U, 168U, 143U, 120U, 98U, 78U, 60U,
		44U, 31U, 20U, 11U, 4U, 1U, 0U, 1U, 5U, 12U, 21U, 32U, 46U, 62U, 80U,
		100U, 122U, 145U, 170U, 196U, 223U, 250U, 278U, 307U, 335U, 363U, 390U,
		417U, 443U, 468U, 491U, 513U, 533U, 551U, 567U, 581U, 593U, 602U, 608U,
		612U, 614U, 613U, 609U, 602U, 594U, 582U, 569U, 553U, 535U, 515U, 493U,
		470U, 446U, 420U, 393U, 366U, 338U, 309U, 281U, 253U, 226U, 199U, 173U,
		148U, 124U, 102U, 82U, 64U, 47U, 33U, 22U, 12U, 6U, 1U, 0U, 0U, 4U, 10U,
		19U, 30U, 43U, 59U, 76U, 96U, 118U, 141U, 165U, 191U, 217U, 245U, 273U,
		301U, 329U, 357U, 385U, 412U, 438U, 463U, 487U, 509U, 529U, 548U, 564U,
		579U, 591U, 600U, 607U, 612U, 614U, 613U, 610U, 604U, 596U, 585U, 572U,
		556U, 539U, 519U, 498U, 475U, 451U, 425U, 399U, 371U, 343U, 315U, 287U,
		259U, 231U, 204U, 178U, 153U, 129U, 107U, 86U, 67U, 51U, 36U, 24U, 14U,
		7U, 2U, 0U, 0U, 3U, 9U, 17U, 27U, 40U, 55U, 73U, 92U, 113U, 136U, 160U,
		186U, 212U, 239U, 267U, 295U, 324U, 352U, 379U, 407U, 433U, 458U, 482U,
		505U, 525U, 544U, 561U, 576U, 588U, 598U, 606U, 611U, 613U, 613U, 611U,
		605U, 597U, 587U, 574U, 560U, 542U, 523U, 502U, 480U, 456U, 430U, 404U,
		377U, 349U, 321U, 292U, 264U, 237U, 209U, 183U, 158U, 134U, 111U, 90U,
		71U, 54U, 39U, 26U, 16U, 8U, 3U, 0U, 0U, 2U, 7U, 15U, 25U, 37U, 52U,
		69U, 88U, 109U, 131U, 155U, 180U, 207U, 234U, 262U, 290U, 318U, 346U,
		374U, 401U, 428U, 453U, 477U, 500U, 521U, 541U, 558U, 573U, 586U, 597U,
		605U, 610U, 613U, 614U, 611U, 607U, 599U, 589U, 577U, 563U, 546U, 527U,
		507U, 484U, 461U, 435U, 409U, 382U, 355U, 326U, 298U, 270U, 242U, 215U,
		188U, 163U, 138U, 115U, 94U, 75U, 57U, 42U, 28U, 18U, 9U, 3U, 0U, 0U,
		2U, 6U, 13U, 23U, 35U, 49U, 66U, 84U, 104U, 127U, 150U, 175U, 201U,
		228U, 256U, 284U, 312U, 341U, 368U, 396U, 423U, 448U, 473U, 496U, 517U,
		537U, 555U, 570U, 584U, 595U, 603U, 609U, 613U, 614U, 612U, 608U, 601U,
		592U, 580U, 566U, 549U, 531U, 511U, 489U, 465U, 441U, 415U, 388U, 360U,
		332U, 304U, 276U, 248U, 220U, 193U, 168U, 143U, 120U, 98U, 78U, 60U,
		44U, 31U, 20U, 11U, 4U, 1U, 0U, 1U, 5U, 12U, 21U, 32U, 46U, 62U, 80U,
		100U, 122U, 145U, 170U, 196U, 223U, 250U, 278U, 307U, 335U, 363U, 390U,
		417U, 443U, 468U, 491U, 513U, 533U, 551U, 567U, 581U, 593U, 602U, 608U,
		612U, 614U, 613U, 609U, 602U, 594U, 582U, 569U, 553U, 535U, 515U, 493U,
		470U, 446U, 420U, 393U, 366U, 338U, 309U, 281U, 253U, 226U, 199U, 173U,
		148U, 124U, 102U, 82U, 64U, 47U, 33U, 22U, 12U, 6U, 1U, 0U, 0U, 4U, 10U,
		19U, 30U, 43U, 59U, 76U, 96U, 118U, 141U, 165U, 191U, 217U, 245U, 273U,
		301U, 329U, 357U, 385U, 412U, 438U, 463U, 487U, 509U, 529U, 548U, 564U,
		579U, 591U, 600U, 607U, 612U, 614U, 613U, 610U, 604U, 596U, 585U, 572U,
		556U, 539U, 519U, 498U, 475U, 451U, 425U, 399U, 371U, 343U, 315U, 287U,
		259U, 231U, 204U, 178U, 153U, 129U, 107U, 86U, 67U, 51U, 36U, 24U, 14U,
		7U, 2U, 0U, 0U, 3U, 9U, 17U, 27U, 40U, 55U, 73U, 92U, 113U, 136U, 160U,
		186U, 212U, 239U, 267U, 295U, 324U, 352U, 379U, 407U, 433U, 458U, 482U,
		505U, 525U, 544U, 561U, 576U, 588U, 598U, 606U, 611U, 613U, 613U, 611U,
		605U, 597U, 587U, 574U, 560U, 542U, 523U, 502U, 480U, 456U, 430U, 404U,
		377U, 349U, 321U, 292U, 264U, 237U, 209U, 183U, 158U, 134U, 111U, 90U,
		71U, 54U, 39U, 26U, 16U, 8U, 3U, 0U, 0U, 2U, 7U, 15U, 25U, 37U, 52U,
		69U, 88U, 109U, 131U, 155U, 180U, 207U, 234U, 262U, 290U, 318U, 346U,
		374U, 401U, 428U, 453U, 477U, 500U, 521U, 541U, 558U, 573U, 586U, 597U,
		605U, 610U, 613U, 614U, 611U, 607U, 599U, 589U, 577U, 563U, 546U, 527U,
		507U, 484U, 461U, 435U, 409U, 382U, 355U, 326U, 298U, 270U, 242U, 215U,
		188U, 163U, 138U, 115U, 94U, 75U, 57U, 42U, 28U, 18U, 9U, 3U, 0U, 0U,
		2U, 6U, 13U, 23U, 35U, 49U, 66U, 84U, 104U, 127U, 150U, 175U, 201U,
		228U, 256U, 284U, 312U, 341U, 368U, 396U, 423U, 448U, 473U, 496U, 517U,
		537U, 555U, 570U, 584U, 595U, 603U, 609U, 613U, 614U, 612U, 608U, 601U,
		592U, 580U, 566U, 549U, 531U, 511U, 489U, 465U, 441U, 415U, 388U, 360U,
		332U, 304U, 276U, 248U, 220U, 193U, 168U, 143U, 120U, 98U, 78U, 60U,
		44U, 31U, 20U, 11U, 4U, 1U, 0U, 1U, 5U, 12U, 21U, 32U, 46U, 62U, 80U,
		100U, 122U, 145U, 170U, 196U, 223U, 250U, 278U, 307U };

/*******************************************************************************
 * Code
 ******************************************************************************/

void MP3_Set_Sample_Rate(mp3_sample_rate_t sr) {
	uint32_t mod_val;
	pdb_divider_multiplication_factor_t mult_fact;
	pdb_prescaler_divider_t prescaler;

	switch (sr) {
	case kMP3_8000Hz:
		mod_val = 11;
		mult_fact = kPDB_DividerMultiplicationFactor10;
		prescaler = kPDB_PrescalerDivider64;
		break;
	case kMP3_11025Hz:
		mod_val = 17;
		mult_fact = kPDB_DividerMultiplicationFactor10;
		prescaler = kPDB_PrescalerDivider32;
		break;
	case kMP3_12000Hz:
		mod_val = 7;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider32;
		break;
	case kMP3_16000Hz:
		mod_val = 11;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider16;
		break;
	case kMP3_22050Hz:
		mod_val = 17;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider8;
		break;
	case kMP3_24000Hz:
		mod_val = 15;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider8;
		break;
	case kMP3_32000Hz:
		mod_val = 11;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider8;
		break;
	case kMP3_44100Hz:
		mod_val = 8;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider8;
		break;
	case kMP3_48000Hz:
		mod_val = 7;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider8;
		break;
	default:
		mod_val = 8;
		mult_fact = kPDB_DividerMultiplicationFactor20;
		prescaler = kPDB_PrescalerDivider8;
		break;
	}

	DAC_Wrapper_PDB_Config(mod_val, mult_fact, prescaler);
}

void PIT_LED_HANDLER(void) {
	/* Clear interrupt flag.*/
	PIT_ClearStatusFlags(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL, kPIT_TimerFlag);
	pitIsrFlag = true;
	/* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
	 * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
	 * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
	 */
	__DSB();
}

uint8_t cambio(uint8_t count);
uint8_t cambiosin(uint8_t count_);

int main(void) {
	/* Structure of initialize PIT */
	pit_config_t pitConfig;

	/* Board pin, clock, debug console init */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	/* Initialize and enable LED */
	LED_INIT();

	DAC_Wrapper_Init();

	//DAC_USED_BUFFER_SIZE pero lo explicito
	DAC_Wrapper_Loop(true);		//Por defecto está en true pero lo explicito

	DAC_Wrapper_Start_Trigger();

	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(DEMO_PIT_BASEADDR, &pitConfig);
	PIT_SetTimerPeriod(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
			USEC_TO_COUNT(1000000U, PIT_SOURCE_CLOCK));
	PIT_EnableInterrupts(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL,
			kPIT_TimerInterruptEnable);
	EnableIRQ(PIT_IRQ_ID);
	PIT_StartTimer(DEMO_PIT_BASEADDR, DEMO_PIT_CHANNEL);

	uint8_t count = 0;
	//bool flag = true;
	DAC_Wrapper_Set_Data_Array(&sen1k, DAC_USED_BUFFER_SIZE);

	while (true) {
		/* Check whether occur interupt and toggle LED */
		if (true == pitIsrFlag) {
			LED_TOGGLE();
			pitIsrFlag = false;
			//count = cambio(count);
		}
	}
}

uint8_t cambio(uint8_t count_) {
	uint8_t count = count_;
	switch (count) {
	case (0):
		//DAC_Wrapper_Set_Data_Array(&sen1k, DAC_USED_BUFFER_SIZE);
		DAC_Wrapper_Set_Freq(22050);
		break;
	case (10):
		//DAC_Wrapper_Set_Data_Array(&sen15k, DAC_USED_BUFFER_SIZE);
		DAC_Wrapper_Set_Freq(44100);
		break;
	case (20):
		//DAC_Wrapper_Set_Data_Array(&sen15k, DAC_USED_BUFFER_SIZE);
		DAC_Wrapper_Set_Freq(88200);
		break;
	}
	if (++count == 30) {
		count = 0;
	}
	return count;
}
