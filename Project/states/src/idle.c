#include "idle.h"
#include "gps.h"
#include "gyroCompass.h"
#include "lcd.h"
#include "threads.h"
#include "circleMatrix.h"
#include "locations.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>

LOG_MODULE_REGISTER(idle);

#define REQUIRED_DIST_METERS 20
#define GAMES_AMOUNT 10

unsigned idleThreadCount = 1;
char *idleThreads[1] = {"ledcircle"};

void getIdleThreads(char ***names, unsigned *amount) {
	*names = idleThreads;
	*amount = idleThreadCount;
}

int playIdle() {
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	// Create and randomize array of coordinates
	int64_t lats[NR_OF_LOCS] = {LAT_LOC_A, LAT_LOC_B, LAT_LOC_C, LAT_LOC_D, LAT_LOC_E, LAT_LOC_F, LAT_LOC_G, LAT_LOC_H, LAT_LOC_I, LAT_LOC_J};
	int64_t lons[NR_OF_LOCS] = {LON_LOC_A, LON_LOC_B, LON_LOC_C, LON_LOC_D, LON_LOC_E, LON_LOC_F, LON_LOC_G, LON_LOC_H, LON_LOC_I, LON_LOC_J};
#endif
	static int locIndex = 0;
	static bool completedGames[GAMES_AMOUNT] = {false, false, false, false, false, false, false, false, false, false};

	// Check if all games have been completed
	bool allGamesFinished = true;
	for (int i = 0; i < GAMES_AMOUNT; i++) {
		if (completedGames[i] == false) {
			allGamesFinished = false;
			break;
		}
	}
	if (allGamesFinished) {
		LOG_INF("All games completed\n");
		return -1;
	}

	while (completedGames[locIndex] == true) {
		locIndex = rand() % 10;	// Keep getting random index until a game which has not been done yet is found
	}
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int distMeters = 100;	// Initialize to a value outside the expected range
	int dir = 0;			// Direction the user must head in
	lcdEnable();
	bool lcdSet = false;
	while(distMeters > REQUIRED_DIST_METERS) {	// Device is too far away from next target
		int64_t currLat = getLatitude();		// Get the current latitude
		int64_t currLon = getLongitude();		// Get the current longitude
		if ( currLat == 0 && currLon == 0) {	// GPS doesn't have lock
			LOG_ERR("GPS does not have a lock!\n");
			lcdStringWrite("GPS heeft geen  fix..");
			k_msleep(500);
			lcdStringWrite("GPS heeft geen  fix...");
			k_msleep(500);
		} else
		{
			if (!lcdSet)
			{
				lcdStringWrite("Volg de LEDs!");
				lcdSet = true;
			}

			distMeters = getDistanceMeters(nanoDegToLdDeg(currLon), nanoDegToLdDeg(currLat), nanoDegToLdDeg(lons[locIndex]), nanoDegToLdDeg(lats[locIndex])); // Distance from current position to next location (meters)
			dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(lats[locIndex]), nanoDegToLdDeg(lons[locIndex]));					// Angle between current location and next location

			set_led_circle_dir_dist(dir, distMeters);	// Set the led circle direction and distance
		}
	}
#endif
	lcdStringWrite("Gearriveerd!!");
	k_msleep(4000);
	completedGames[locIndex] = true;
	return locIndex;
}