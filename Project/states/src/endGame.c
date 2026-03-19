//
// Created by Kasper Janssen on 07/06/25.
//

#include "endGame.h"
#include "helperFunctions.h"
#include "lcd.h"
#include "sdCard.h"
#include "sevenSegment.h"
#include "locations.h"

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <stdio.h>

LOG_MODULE_REGISTER(endGame);

#define End_Game_ONELINERS 2
char oneLinersEndGame[End_Game_ONELINERS][32] = {
    "Het spel is nu    afgelopen  ",
    "Bedankt voor het spelen!     "
};

unsigned endGameThreadCount = 2;
char *endGameThreads[2] = {"ledcircle", "abcbtn"};

void getEndGameThreads(char ***names, unsigned *amount) {
    *names = endGameThreads;
    *amount = endGameThreadCount;
}

void playEndGame()
{
    int score = 0;
    char score_string[16];
    uint8_t input_count = 0;
    uint8_t* abcbtns;

    show_oneliners(oneLinersEndGame , End_Game_ONELINERS);
    lcdEnable();

    score = sd_get_score();
    snprintf(score_string, sizeof(score_string), "Score: %d", score);
    lcdStringWrite(score_string);
    //show score till all abc buttons are pressed at same time
    while (input_count != 3) {
        abcbtns = abcbtnGetMutexValue();
        input_count = 0;

        for (int i = 0; i < 3; i++){
			if (abcbtns[i] == 0){
				input_count++;
			}
		}
		native_loop();
    }
    lcdClear();
	lcdDisable();
}