#include "idle.h"
#include "gps.h"
#include "gyroCompass.h"
#include "lcd.h"
#include "threads.h"
#include "circleMatrix.h"
#include "locations.h"
#include "helperFunctions.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>

LOG_MODULE_REGISTER(idle);

#define REQUIRED_DIST_METERS 20
#define GAMES_AMOUNT 10
#define NR_OF_LOCS 64

#if defined(CONFIG_TESTMODE)
unsigned idleThreadCount = 2;
char *idleThreads[2] = {"ledcircle", "abcbtn"};
#else
unsigned idleThreadCount = 1;
char *idleThreads[1] = {"ledcircle"};
#endif

void getIdleThreads(char ***names, unsigned *amount) {
	*names = idleThreads;
	*amount = idleThreadCount;
}

static struct locations {
	int64_t lat;
	int64_t lon;
	uint8_t mg_id;
} locations[NR_OF_LOCS] = {0};

static size_t locations_count = 0;

// TODO: Add minigame ID's to the locations struct
void initLocations()
{
	struct Location *locArray = NULL;
	char lcd_msg[32];
	int ret = locations_load(1, &locArray, &locations_count, NR_OF_LOCS);
	if (ret != 0)
	{
		snprintf(lcd_msg, sizeof(lcd_msg), "Init loc err: %d", ret);
		lcdEnable();
		lcdStringWrite(lcd_msg);
		k_msleep(2000);
		LOG_ERR("Failed to load locations from disk");
		return;
	}

	for (int i = 0; i < locations_count; i++)
	{
		locations[i].lat = locArray[i].x;
		locations[i].lon = locArray[i].y;
		locations[i].mg_id = locArray[i].mg_id < GAMES_AMOUNT ? locArray[i].mg_id : -2; // -2 if no minigame is assigned or out of range
		LOG_INF("Location %d: lat=%lld, lon=%lld, mg_id=%d", i, locations[i].lat, locations[i].lon, locations[i].mg_id);
	}
}

int playIdle() {
#if defined(CONFIG_TESTMODE)
	static int testIndex = 1;
	char lcd_msg[32];

	lcdEnable();
	lcdStringWrite("Selecteer een spel met A en C");
	k_msleep(3000);
	lcdStringWrite("Bevestig met de startknop!");
	k_msleep(3000);

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	startledSet(1);
	sprintf(lcd_msg, "Minigame: %d", testIndex);
	lcdStringWrite(lcd_msg);

	while(startbuttonGet()){
		native_loop();
		wait_till_abc_depressed();

		if(abcbuttonsGet('a') == 0){
			if(testIndex == 1){
				testIndex = 10;
			}
			else{
				testIndex--;
			}
			sprintf(lcd_msg, "Minigame: %d", testIndex);
			lcdStringWrite(lcd_msg);
		} 
		else if(abcbuttonsGet('c') == 0){
			if(testIndex == 10){
				testIndex = 1;
			}
			else{
				testIndex++;
			}
			sprintf(lcd_msg, "Minigame: %d", testIndex);
			lcdStringWrite(lcd_msg);
		}
	}

	startledSet(0);
	abcledsSet('a', 0);
	abcledsSet('c', 0);
	lcdClear();
	lcdDisable();
	k_msleep(100);
	//convert the testIndex from human 1-10 back to code's 0-9
	testIndex--;

	return testIndex;
#endif

#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	static bool firstTimeIdle = true; // i know should be in init but this is easier for now.
	if (firstTimeIdle) // Check if locations are initialized
	{
		firstTimeIdle = false;
		initLocations();
	}
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

	if (locIndex >= locations_count)
	{
		LOG_INF("All locations have been visited.");
		return -1;
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

			distMeters = getDistanceMeters(nanoDegToLdDeg(currLon), nanoDegToLdDeg(currLat), nanoDegToLdDeg(locations[locIndex].lat), nanoDegToLdDeg(locations[locIndex].lon)); // Distance from current position to next location (meters)
			dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(locations[locIndex].lat), nanoDegToLdDeg(locations[locIndex].lon));					// Angle between current location and next location

			set_led_circle_dir_dist(dir, distMeters);	// Set the led circle direction and distance
		}
	}
#endif
	lcdStringWrite("Gearriveerd!!");
	k_msleep(4000);
	completedGames[locIndex] = true;
	locIndex++; // Increment the index to the  next location.
	return locations[locIndex--].mg_id; // Return the index of the game that has to be played.
}