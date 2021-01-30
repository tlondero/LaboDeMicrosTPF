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
#define FILTERS_NUMBER      18

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

static int16_t eq_clasic_db[BAND_PASS_NUMBER] = { 0, 0, 0, 0, 0, 0, -71, -71,
		-71, -96 };

static int16_t eq_club_db[BAND_PASS_NUMBER] = { 0, 0, 79, 55, 55, 55, 32, 0, 0,
		0 };

static int16_t eq_dance_db[BAND_PASS_NUMBER] = { 96, 71, 24, 0, 0, -55, -71.
		- 71, 0, 0 };

static int16_t eq_bass_db[BAND_PASS_NUMBER] = { -79, 96, 96, 55, 16, -39, -79,
		-103, -111, -111 };

static int16_t eq_bass_and_treble_db[BAND_PASS_NUMBER] = { 71, 55, 0, -71, -48,
		16, 79, 111, 119, 119 };

static int16_t eq_trable_db[BAND_PASS_NUMBER] = { -96, -96, -96, -39, 24, 111,
		159, 159, 159, 167 };

static int16_t eq_headset_db[BAND_PASS_NUMBER] = { 48, 111, 55, -32, -24, 16,
		48, 96, 128, 143 };

static int16_t eq_hall_db[BAND_PASS_NUMBER] = { 103, 103, 55, 55, 0, -48, -48,
		-48, 0, 0 };

static int16_t eq_live_db[BAND_PASS_NUMBER] = { -48, 0, 39, 55, 55, 55, 39, 24,
		24, 24 };

static int16_t eq_party_db[BAND_PASS_NUMBER] = { 71, 71, 0, 0, 0, 0, 0, 0, 71,
		71 };

static int16_t eq_pop_db[BAND_PASS_NUMBER] = { -16, 48, 71, 79, 55, 0, -24, -24,
		-16, -16 };

static int16_t eq_reggae_db[BAND_PASS_NUMBER] = { 0, 0, 0, -55, 0, 64, 64, 0, 0,
		0 };

static int16_t eq_rock_db[BAND_PASS_NUMBER] = { 79, 48, -55, -79, -32, 39, 88,
		111, 111, 111 };

static int16_t eq_ska_db[BAND_PASS_NUMBER] = { -24, -48, -39, 0, 39, 55, 88, 96,
		111, 96 };

static int16_t eq_soft_db[BAND_PASS_NUMBER] = { 48, 16, 0, -24, 0, 39, 79, 96,
		111, 119 };

static int16_t eq_soft_rock_db[BAND_PASS_NUMBER] = { 39, 39, 24, 0, -39, -55,
		-32, 0, 24, 88 };

static int16_t eq_techno_db[BAND_PASS_NUMBER] = { 79, 55, 0, -55, -48, 0, 79,
		96, 96, 88 };

static int16_t *presets[FILTERS_NUMBER] = { eq_off_db, eq_clasic_db, eq_club_db,
		eq_dance_db, eq_bass_db, eq_bass_and_treble_db, eq_trable_db,
		eq_headset_db, eq_hall_db, eq_live_db, eq_party_db, eq_pop_db,
		eq_reggae_db, eq_rock_db, eq_ska_db, eq_soft_db, eq_soft_rock_db,
		eq_techno_db };

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

#endif /* EQUALISER_PRESETS_H_ */
