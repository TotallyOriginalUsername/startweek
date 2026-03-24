#include "idle.h"

#include <sdCard.h>

#include "gps.h"
#include "gyroCompass.h"
#include "lcd.h"
#include "threads.h"
#include "circleMatrix.h"
#include "locations.h"
#include "helperFunctions.h"
#include "zephyr/sys/printk.h"
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>

LOG_MODULE_REGISTER(idle);

#define REQUIRED_DIST_METERS 20
#define GAMES_AMOUNT 10
#define NR_OF_LOCS 64

unsigned idleThreadCount = 2;
char *idleThreads[2] = {"ledcircle", "abcbtn"};

void getIdleThreads(char ***names, unsigned *amount) {
	*names = idleThreads;
	*amount = idleThreadCount;
}

static struct location_new locationsV2[NR_OF_LOCS] = {0};
static int location_countV2 = 0;
static int locIndex = 0;
static int lastReturned = -2; // -2 if never returned, -1 if all locations visited >= 0 is the last mg_id

void reloadLocations(){
	memset(locationsV2, 0, sizeof locationsV2);
	sd_get_locations(locationsV2, sizeof(locationsV2), &location_countV2);
	for(int i = 0; i < location_countV2; i++){
		locationsV2[i].lat = locationsV2[i].lat * 1000; // convert back to nanodegrees
        locationsV2[i].lon = locationsV2[i].lon * 1000;
	}
}

void reloadProgress() {
  locIndex =  sd_get_progress();   // Get the current location index from the SD card
  if (locIndex == -ENOENT) // If the file doesn't exist set locIndex to 0 and create the file.
  {
    lcdEnable();
    lcdStringWrite("Geen voortgang gevonden");
    k_msleep(1000);
    locIndex = 0;
	lastReturned = -2;
    sd_set_progress(locIndex); // If no progress file exists, set it to 0
    lcdStringWrite("Voortgang gereset");
    k_msleep(1000);
  } 
}

bool checkDevInput(){
	uint8_t* abcbtns;
	abcbtns = abcbtnGetMutexValue();
    uint8_t input_count = 0;

    for (int i = 0; i < 3; i++){
		if (abcbtns[i] == 0){
			input_count++;
		}
	}
	if(input_count >= 3){
		return true;
	}
	else {
		return false;
	}
}

int playIdle(uint8_t* trivia_ID, bool* devModeOn) {
	uint8_t ledcircleOff[8] = {0};
	lcdEnable();

	// If returning from a minigame, increment and save progress
	if (lastReturned >= 0) {
		locIndex = sd_get_progress();
		locIndex++;
		sd_set_progress(locIndex);
	}

	if (locIndex >= location_countV2) {
		LOG_INF("All locations have been visited.");
		lastReturned = -1;
		return lastReturned;
	}

#if defined(CONFIG_NOGPSMODE)
	if(checkDevInput()){
		*devModeOn = true;
		return 0;
	}

#elif defined(CONFIG_BOARD_NUCLEO_H743ZI)
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

			distMeters = getDistanceMeters(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(locationsV2[locIndex].lat), nanoDegToLdDeg(locationsV2[locIndex].lon)); // Distance from current position to next location (meters)
			dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(locationsV2[locIndex].lat), nanoDegToLdDeg(locationsV2[locIndex].lon));					// Angle between current location and next location

			set_led_circle_dir_dist(get_relative_dir(dir), distMeters);	// Set the led circle direction and distance
		}
		//check for secret dev input
		if(checkDevInput()){
			*devModeOn = true;
			break;
		}
	}
#endif
	lcdStringWrite("Gearriveerd!!");
	ledcircleSetMutexValue(ledcircleOff);
	k_msleep(5000);
	lcdDisable();
	*trivia_ID = locationsV2[locIndex].trivia_id;
	lastReturned = locationsV2[locIndex].mg_id;
	return lastReturned; // Return the index of the game that has to be played.
}
