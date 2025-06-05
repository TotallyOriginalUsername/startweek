//
// Created by Kasper Janssen on 04/06/25.
//
#include "locations.h"

#include <stdlib.h>
#include <zephyr/data/json.h>
#include "gps.h"
#include "sdCard.h"
#include "threads.h"

LOG_MODULE_REGISTER(locations);

#define MAX_TYPES (1 << (sizeof(uint16_t) * 8))
#define BUFFER_SIZE 512 // dirty to have this here.

// LED circle direction to location related definitions
#define ANGLE_BUFFER_SIZE 10
#define DIST_MAX_WIDTH 20	// Distance at which the circle has minimum width
#define DIST_MIN_WIDTH 100 // Was 1000	//Distance at which the circle has maximum width
#define DIST_RANGE (DIST_MIN_WIDTH - DIST_MAX_WIDTH)
#define LEDS_MIN_WIDTH 3	// Amount of LEDS at DIST_MIN_WIDTH
#define LEDS_MAX_WIDTH 32	// Amount of LEDS at DIST_MAX_WIDTH
#define LEDS_RANGE_WIDTH (LEDS_MAX_WIDTH - LEDS_MIN_WIDTH)

/**
 * @brief Load locations from a file based on the type.
 *
 * @param type The type of locations to load.
 * @param locations Pointer to an array of Location pointers to be filled.
 * @param count Pointer to a size_t variable to store the number of loaded locations.
 * @param maxLocations Maximum number of locations to load.
 * @return 0 on success, negative error code on failure.
 * @return 1 if locations of this type are already loaded.
 * @note Possible leak if locations are already loaded and this function is called again.
 */
int locations_load(uint16_t type, struct Location **locations, size_t *count, size_t maxLocations)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    char json_buf[BUFFER_SIZE];
    size_t len = 0;
    int ret = sd_get_locations(type, json_buf, &len, BUFFER_SIZE);
    if (ret != 0)
        return ret;

    struct Location *locArray = malloc(sizeof(struct Location) * maxLocations);
    if (!locArray)
        return -2;

    struct json_obj_descr loc_descr[] = {
        JSON_OBJ_DESCR_PRIM(struct Location, x, JSON_TOK_NUMBER),
        JSON_OBJ_DESCR_PRIM(struct Location, y, JSON_TOK_NUMBER),
    };

    struct json_obj json_arr;
    ret = json_arr_separate_object_parse_init(&json_arr, json_buf, len);
    if (ret < 0) {
        LOG_ERR("Parse init error: %d", ret);
        free(locArray);
        return ret;
    }

    size_t i = 0;
    for (; i < maxLocations; ++i) {
        memset(&locArray[i], 0, sizeof(struct Location));
        ret = json_arr_separate_parse_object(&json_arr, loc_descr, 2, &locArray[i]);
        if (ret == 0) break; // End of array
        if (ret < 0) {
            LOG_ERR("Parse error at index %zu: %d", i, ret);
            // Free allocated locations before returning
            free(locArray);
            return ret;
        }
    }

    *count = i;
    *locations = locArray;
#endif
    return 0;
}

int get_led_width(int distance) {
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

	static double angleSinBuffer[ANGLE_BUFFER_SIZE];
	static double angleCosBuffer[ANGLE_BUFFER_SIZE];

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

/**
 * @brief Function to set the led circle direction and distance for showing where the user should go next.
 * @param dir The direction in degrees (0-359) where the LEDs should be centered.
 * @param distance
 */
void set_led_circle_dir_dist(unsigned dir, int distance)
{
	const int nrPixels = 64;
	const int maxDir = 359;
	unsigned width = get_led_width(distance); // Convert distance to width in pixels

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
			//LOG_WRN("Byte %d bit %d (bitindex %d): %d\n", byteCount, bitCount, bitIndex, enableLed);
			if (enableLed) {
				outputValues[byteCount] |= (1 << (7-bitCount));
			}

		}
	}
	ledcircleSetMutexValue(outputValues);
}