#include "idle.h"

#include <sdCard.h>

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
	uint8_t ledcircleOff[8] = {0};

#if defined(CONFIG_TESTMODE)
	static int testIndex = 0;
	char lcd_msg[32];

	//convert the testIndex from code's 0-9 back to human 1-10
	testIndex++;
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
	//testIndex--;

	return testIndex;
#endif
	static int locIndex = 0;
	static int lastReturned = -2; // -2 if never returned, -1 if all locations visited >= 0 is the last mg_id
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	static bool firstTimeIdle = true; // i know should be in init but this is easier for now.
	if (firstTimeIdle) // Check if locations are initialized
	{
		firstTimeIdle = false;
		initLocations();
		locIndex = sd_get_progress(); // Get the current location index from the SD card in the event that the device was turned off.
		if (locIndex == -ENOENT) // If the file doesn't exist set locIndex to 0 and create the file.
		{
			lcdEnable();
			lcdStringWrite("Geen voortgang gevonden");
			k_msleep(1000);
			locIndex = 0;
			sd_set_progress(locIndex); // If no progress file exists, set it to 0
			lcdStringWrite("Voortgang gereset");
			k_msleep(1000);
		}
		else if (locIndex < 0)
		{
			return locIndex; // If the progress is negative there has been a reading error.
		}
	}
#endif

	// If returning from a minigame, increment and save progress
	if (lastReturned >= 0) {
		locIndex++;
		sd_set_progress(locIndex);
	}

	if (locIndex >= locations_count) {
		LOG_INF("All locations have been visited.");
		lastReturned = -1;
		return lastReturned;
	}

#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int distMeters = 100;	// Initialize to a value outside the expected range
	int dir = 0;			// Direction the user must head in
	char lcd_distance_msg[32];
	lcdEnable();
	bool lcdSet = false;
	while(distMeters > REQUIRED_DIST_METERS) {	// Device is too far away from next target
		int64_t currLat = getLatitude();		// Get the current latitude
		int64_t currLon = getLongitude();		// Get the current longitude
		if ( currLat == 0 && currLon == 0) {	// GPS doesn't have lock
			LOG_ERR("GPS does not have a lock! Lat: %llu Lon: %llu\n", currLat, currLon);
			lcdStringWrite("GPS heeft geen  fix..");
			k_msleep(500);
			lcdStringWrite("GPS heeft geen  fix...");
			k_msleep(500);
		} else
		{
			
			sprintf(lcd_distance_msg, "Volg de LEDs!  %d meter", distMeters);
			lcdStringWrite(lcd_distance_msg);
			k_msleep(50);
			lcdSet = true;

			distMeters = getDistanceMeters(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(locations[locIndex].lat), nanoDegToLdDeg(locations[locIndex].lon)); // Distance from current position to next location (meters)
			dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(locations[locIndex].lat), nanoDegToLdDeg(locations[locIndex].lon));					// Angle between current location and next location

			set_led_circle_dir_dist(get_relative_dir(dir), distMeters);	// Set the led circle direction and distance
		}
	}
#endif
	lcdStringWrite("Gearriveerd!!");
	ledcircleSetMutexValue(ledcircleOff);
	k_msleep(5000);
	lastReturned = locations[locIndex].mg_id;
	return lastReturned; // Return the index of the game that has to be played.
}
