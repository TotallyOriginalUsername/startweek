#include "idle.h"
#include "hardware.h"
#include "threads.h"
#include <zephyr/kernel.h>
#include <stdio.h>

#define REQUIRED_DIST_METERS 20
#define ANGLE_BUFFER_SIZE 10
#define GAMES_AMOUNT 3
#define DIST_MAX_WIDTH 20	// Distance at which the circle has minimum width
#define DIST_MIN_WIDTH 100 // Was 1000	//Distance at which the circle has maximum width
#define DIST_RANGE (DIST_MIN_WIDTH - DIST_MAX_WIDTH)
#define LEDS_MIN_WIDTH 7	// Amount of LEDS at DIST_MIN_WIDTH
#define LEDS_MAX_WIDTH 32	// Amount of LEDS at DIST_MAX_WIDTH
#define LEDS_RANGE_WIDTH (LEDS_MAX_WIDTH - LEDS_MIN_WIDTH)

unsigned idleThreadCount = 1;
char *idleThreads[1] = {"ledcircle"};
double angleSinBuffer[ANGLE_BUFFER_SIZE];
double angleCosBuffer[ANGLE_BUFFER_SIZE];

void getIdleThreads(char ***names, unsigned *amount) {
	*names = idleThreads;
	*amount = idleThreadCount;
}

int playIdle() {
	/******/
	printk("Entered idle");
	static int locIndex = 0;
	//locIndex = rand() % GAMES_AMOUNT;
	static bool completedGames[GAMES_AMOUNT] = {false, false, false};
	while (completedGames[locIndex] == true) {
		locIndex++;
	}
	if (locIndex > 2) { return -1; }

	// // Check if all games have been completed
	// for (int i = 0; i < GAMES_AMOUNT; i++) {
	// 	if (completedGames[GAMES_AMOUNT] == false) {
	// 		break;
	// 	}
	// 	printf("All games completed\n");
	// 	return -1;
	// }

	// while (completedGames[locIndex] == true) {
	// 	locIndex = rand() % 10;	// Keep getting random index until a game which has not been done yet is found
	// }

	int distMeters = 100;	// Initialize to a value outside the expected range
	int dir = 0;			// Direction the user must head in
/* 	lcdEnable();
	bool lcdSet = false;
	while(distMeters > REQUIRED_DIST_METERS) {	// Device is too far away from next target
		int64_t currLat = getLatitude();		// Get the current latitude
		int64_t currLon = getLongitude();		// Get the current longitude
		if ( currLat == 0 && currLon == 0) {	// GPS doesn't have lock
			//printf("GPS does not have a lock!\n");
			lcdStringWrite("GPS heeft geen  fix..");
			k_msleep(500);
			lcdStringWrite("GPS heeft geen  fix...");
			k_msleep(500);
			continue;
		} else if (!lcdSet) {
			lcdStringWrite("Volg de LEDs!");
			lcdSet = true;
		}
		distMeters = getDistanceMeters(nanoDegToLdDeg(currLon), nanoDegToLdDeg(currLat), nanoDegToLdDeg(lons[locIndex]), nanoDegToLdDeg(lats[locIndex])); // Distance from current position to next location (meters)
		dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(lats[locIndex]), nanoDegToLdDeg(lons[locIndex]));					// Angle between current location and next location
		//printf("dir: %d\n", dir);
		int compassDir;
		gyroCompass_get_heading(&compassDir);	// Angle of device */
		
		//char banana[25];
		//char distValue[3];
		//strcpy(banana, "Distance: ");
		//sprintf(distValue, "%d", distMeters);
		//strcat(banana,distValue);
		//strcat(banana, " angle: ");
		//char rotationValue[3];
		//sprintf(rotationValue, "%d", compassDir);
		//strcat(banana, rotationValue);

/* 		compassDir = circleMovingAvg(compassDir);
		
		dir = dir - compassDir;					// Add to get the direction compared to the device
		if (dir < 0 ) {
			dir += 360;
		} else if (dir >= 360) {
			dir -= 360;
		}

		int ledWidth = getLedWidth(distMeters);

		setLedCircleDirWidth(dir, ledWidth);			// Point the user in the correct direction
		if (distMeters < 20) {
			break;
		}
		k_msleep(1);
	} */
	//lcdStringWrite("Gearriveerd!!");
	k_msleep(4000);
	completedGames[locIndex] = true;
	printk("left idle");
	return ((locIndex+1)*2)-1;
	//}
}