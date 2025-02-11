#include "idle.h"
#include "gps.h"
#include "gyroCompass.h"
#include "lcd.h"
#include "threads.h"
#include "circleMatrix.h"
#include "locations.h"
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

int getLedWidth(int distance) {
	if (distance > DIST_MIN_WIDTH) { return LEDS_MIN_WIDTH; }
	if (distance < DIST_MAX_WIDTH) { return LEDS_MAX_WIDTH; }
	distance -= DIST_MAX_WIDTH;
	double distFraction = (double)distance / DIST_RANGE;
	distFraction = 1 - distFraction;
	return LEDS_RANGE_WIDTH*distFraction + LEDS_MIN_WIDTH;
}

int circleMovingAvg(int newValue) {
	double rad = toRadians(newValue);
	double sinVal = sin(rad);
	double cosVal = cos(rad);

	for (int i = ANGLE_BUFFER_SIZE-1; i > 0; i--) {	// Move every entry in the buffer up by one index
		angleSinBuffer[i] = angleSinBuffer[i-1];
		angleCosBuffer[i] = angleCosBuffer[i-1];
	}
	angleSinBuffer[0] = sinVal;						// Insert the new value
	angleCosBuffer[0] = cosVal;

	double sinSum = 0;
	double cosSum = 0;
	for (int i = 0; i < ANGLE_BUFFER_SIZE; i++) {
		sinSum += angleSinBuffer[i];
		cosSum += angleCosBuffer[i];
	}
	double sinAvg = sinSum / ANGLE_BUFFER_SIZE;
	double cosAvg = cosSum / ANGLE_BUFFER_SIZE;
	return toDegrees(atan2(sinAvg,cosAvg));
}

void getIdleThreads(char ***names, unsigned *amount) {
	*names = idleThreads;
	*amount = idleThreadCount;
}

int playIdle() {
	/******/
	// Create and randomize array of coordinates
	int64_t lats[NR_OF_LOCS] = {LAT_LOC_A, LAT_LOC_B, LAT_LOC_C};
	int64_t lons[NR_OF_LOCS] = {LON_LOC_A, LON_LOC_B, LON_LOC_C};
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
	lcdEnable();
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
		gyroCompass_get_heading(&compassDir);	// Angle of device
		
		//char banana[25];
		//char distValue[3];
		//strcpy(banana, "Distance: ");
		//sprintf(distValue, "%d", distMeters);
		//strcat(banana,distValue);
		//strcat(banana, " angle: ");
		//char rotationValue[3];
		//sprintf(rotationValue, "%d", compassDir);
		//strcat(banana, rotationValue);

		compassDir = circleMovingAvg(compassDir);
		
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
	}
	lcdStringWrite("Gearriveerd!!");
	k_msleep(4000);
	completedGames[locIndex] = true;
	return ((locIndex+1)*2)-1;
	//}
}

void setLedCircleDirWidth(unsigned dir, unsigned width) {
	// dir must be in degrees (so zero through 359)
	// widtb: total width of the 'band' in pixels

	const int nrPixels = 64;
	const int maxDir = 359;
	float centerFloat = (float)dir / maxDir;
	int centerPixel = round(centerFloat * nrPixels);

	int leftBound = centerPixel - (width / 2);
	if (leftBound < 0 ) {
		leftBound = nrPixels + leftBound;
	}
	int rightBound = centerPixel + (width / 2);
	if (rightBound > (nrPixels - 1)) {
		rightBound = rightBound - nrPixels;
	}
	bool overlap = false;
	if (leftBound > rightBound) {
		overlap = true;
	}

	const int nrBytes = 8;
	const int nrBits = 8;
	uint8_t outputValues[8] = {0,0,0,0,0,0,0,0};	// Initialize to zero (all LEDs off)
	for (int byteCount = 0; byteCount < nrBytes; byteCount++) {
		for (int bitCount = 0; bitCount < nrBits; bitCount++) {
			int bitIndex = 8 * byteCount + bitCount;
			//bool enableLed = (!overlap && (bitIndex >= leftBound) && (bitIndex >= rightBound)) || (overlap && (bitIndex >= leftBound || bitIndex <= rightBound));
			bool enableLed = false;
			if (!overlap) {
				if (bitIndex >= leftBound && bitIndex <= rightBound) {
					enableLed = true;
				}
			} else {
				if (bitIndex >= leftBound || bitIndex <= rightBound) {
					enableLed = true;
				}
			}
			//printf("Byte %d bit %d (bitindex %d): %d\n", byteCount, bitCount, bitIndex, enableLed);
			if (enableLed) {
				outputValues[byteCount] |= (1 << (7-bitCount));
			}

		}
	}
	ledcircleSetMutexValue(outputValues);
}

void ledCircleDemo() {
	// Just a small demo to show off the led circle in all its glory
	for (int i = 6; i < 40; i++) {
		setLedCircleDirWidth(0, i);
		k_msleep(4);
	}
	k_msleep(400);
	for (int i = 39; i >= 6; i--) {
		setLedCircleDirWidth(0, i);
		k_msleep(4);
	}
	k_msleep(400);
	for (int i = 0; i < 360; i++) {
		setLedCircleDirWidth(i, 10);
		k_msleep(2);
	}
	k_msleep(200);
	for (int i = 359; i >= 0; i--) {
		setLedCircleDirWidth(i, 10);
		k_msleep(2);
	}
	k_msleep(300);
}