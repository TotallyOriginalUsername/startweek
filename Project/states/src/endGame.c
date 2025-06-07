//
// Created by Kasper Janssen on 07/06/25.
//

#include "endGame.h"
#include "helperFunctions.h"
#include "sdCard.h"
#include "sevenSegment.h"
#include "locations.h"

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <stdio.h>

LOG_MODULE_REGISTER(endGame);

#define REQUIRED_DIST_METERS 20

#define End_Game_ONELINERS 3
char oneLinersEndGame[End_Game_ONELINERS][32] = {
    "Het spel is nu    afgelopen  ",
    "Bedankt voor het spelen!     ",
    "Ga terug naar de startlocatie",
};

#if defined(CONFIG_TESTMODE)
unsigned endGameThreadCount = 2;
char *endGameThreads[2] = {"ledcircle", "abcbtn"};
#else
unsigned endGameThreadCount = 1;
char *endGameThreads[1] = {"ledcircle"};
#endif

void getEndGameThreads(char ***names, unsigned *amount) {
    *names = endGameThreads;
    *amount = endGameThreadCount;
}

void playEndGame()
{
    int score = 0;
    char score_string[5];
    int distMeters = 100;	// Initialize to a value outside the expected range
    int dir = 0;			// Direction the user must head in

    score = sd_get_score();
    snprintf(score_string, sizeof(score_string), "%04d", score);

    sevenSegmentSet(score_string, 0);

    while(distMeters > REQUIRED_DIST_METERS) {	// Device is too far away from next target
        show_oneliners(oneLinersEndGame, End_Game_ONELINERS);

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
            distMeters = getDistanceMeters(nanoDegToLdDeg(currLon), nanoDegToLdDeg(currLat), nanoDegToLdDeg(LON_LOC_AVANS), nanoDegToLdDeg(LON_LOC_AVANS));    // Distance from current position to next location (meters)
            dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(LON_LOC_AVANS), nanoDegToLdDeg(LON_LOC_AVANS));					                        // Angle between current location and next location

            set_led_circle_dir_dist(dir, distMeters);	// Set the led circle direction and distance
        }
    }
}