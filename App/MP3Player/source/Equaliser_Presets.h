/***************************************************************************//**
 @file     Equaliser_Presets.h
 @brief	///
 @author   MAGT
 ******************************************************************************/
#ifndef EQUALISER_PRESETS_H_
#define EQUALISER_PRESETS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BAND_PASS_NUMBER    10
#define FILTERS_NUMBER      10

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

//TODO
/*******************************************************************************
 * En vez de ir desde 19.9 a -19.9 hice que vaya de 199 a -199
 * Es decir que para tenerlo en db habría que dividir cada numero por 10
 * Es para evitar floats por el momento, depues vemos que onda como lo arreglamos
 ******************************************************************************/

static int16_t eq_off_db[BAND_PASS_NUMBER] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static int16_t eq_clasic_db[BAND_PASS_NUMBER] = { 0, 0, 0, 0, 0, 0, -110, -110, -110, -150 };

static int16_t eq_club_db[BAND_PASS_NUMBER] = { 0, 0, 110, 80, 80, 80, 50, 0, 0,
		0 };

static int16_t eq_dance_db[BAND_PASS_NUMBER] = { 130, 110, 50, 0, 0, -70, -110, -110, 0, 0 };

static int16_t eq_bass_db[BAND_PASS_NUMBER] = { 110, 180, 180, 150, 30, -70, -70, -150, -180, -180 };

static int16_t eq_bass_and_treble_db[BAND_PASS_NUMBER] = { 79, 80, 0, -150, -100, 20, 79, 111, 119, 119 };

static int16_t eq_trable_db[BAND_PASS_NUMBER] = { -96, -96, -96, -39, 24, 111, 159, 159, 159, 167 };

static int16_t eq_headset_db[BAND_PASS_NUMBER] = { 100, 150, 70, -50, -40, 30, 70, 110, 158, 163 };

static int16_t eq_hall_db[BAND_PASS_NUMBER] = { 153, 153, 110, 110, 0, -65, -65, -48, 0, 0 };

static int16_t eq_live_db[BAND_PASS_NUMBER] = { -48, 0, 39, 55, 55, 55, 39, 24, 24, 24 };

static int16_t eq_party_db[BAND_PASS_NUMBER] = { 110, 110, 0, 0, 0, 0, 0, 0, 110, 110 };

static int16_t eq_pop_db[BAND_PASS_NUMBER] = { -30, 60, 100, 110, 70, 0, -40, -40, -26, -26 };

static int16_t eq_reggae_db[BAND_PASS_NUMBER] = { 0, 0, 0, -96, 0, 82, 80, 0, 0, 0 };

static int16_t eq_rock_db[BAND_PASS_NUMBER] = { 110, 70, -80, -110, -50, 64, 120, 156, 156, 156 };

static int16_t eq_ska_db[BAND_PASS_NUMBER] = { -24, -48, -39, 0, 39, 55, 88, 96, 111, 96 };

static int16_t eq_soft_db[BAND_PASS_NUMBER] = { 70, 31, 0, -44, 0, 61, 105, 115, 150, 160 };

static int16_t eq_soft_rock_db[BAND_PASS_NUMBER] = { 62, 62, 43, 0, -70, -100, -50, 0, 43, 130 };

static int16_t eq_techno_db[BAND_PASS_NUMBER] = { 110, 80, 0, -70, -74, 0, 110, 150, 150, 130 };

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

#endif /* EQUALISER_PRESETS_H_ */
