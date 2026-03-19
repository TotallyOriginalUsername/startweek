// Includes: own header file, hardware headers, minigame headers, framework headers, system headers (sorted alpabetically)
#include "statemachine.h"
#include "threads.h"
#include "sdCard.h"
#include "gps.h"

#include "endGame.h"
#include "idle.h"
#include "minigame1.h"
#include "minigame2.h"
#include "minigame3.h"
#include "minigame4.h"
#include "minigame5.h"
#include "minigame6.h"
#include "minigame7.h"
#include "minigame8.h"
#include "minigame9.h"
#include "minigame10.h"
#include "minigame11.h"
#include "minigame12.h"
#include "trivia.h"

#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include <stdio.h>

LOG_MODULE_REGISTER(statemachine);

typedef enum {init_state, idle_state, end_game_state, exit_state, mg_state, trivia_state} statemachineStates;
static int16_t end_time;
static bool firstTimeMG = true;

void getMgThreads(char*** names, unsigned* amount, int mgID){
	switch(mgID){
		case 1:
            getMg1Threads(names, amount);
            break;
        case 2:
            getMg2Threads(names, amount);
            break;
        case 3:
            getMg3Threads(names, amount);
            break;
        case 4:
            getMg4Threads(names, amount);
            break;
        case 5:
            getMg5Threads(names, amount);
            break;
        case 6:
            getMg6Threads(names, amount);
            break;
        case 7:
            getMg7Threads(names, amount);
            break;
        case 8:
            getMg8Threads(names, amount);
            break;
        case 9:
            getMg9Threads(names, amount);
            break;
        case 10:
            getMg10Threads(names, amount);
            break;
		case 11:
            getMg11Threads(names, amount);
            break;
		case 12:
			getMg12Threads(names, amount);
			break;
        default:
            break;
    }
}

// State functions
void init_stateFunction(statemachineStates* next_state) {
	LOG_INF("Initialization\n");
	disableAllThreads();
	uint8_t ret = 0;
	ret = configure();
	if(ret != 0)
	{
		while(true)
		{
			__asm__ volatile ("nop");
		}
	}
	initialize();
	Startupdelay = 0;
#if !defined(CONFIG_TESTMODE) && !defined(CONFIG_NOTIMEMODE)
	// check if current time is the same as the start time
	int16_t start_time = 0;
	int16_t current_time = 0;
	lcdEnable();
	while (current_time < start_time || current_time == 0)
	{
		native_loop();
		start_time = sd_get_start_time();
		if (start_time < 0) {
			LOG_ERR("Start time not set, exiting state machine");
			*next_state = exit_state;
		}

		int16_t hour = getHour();
		int16_t minute = getMinute();
		current_time = (int16_t)((hour * 60) + minute);
		// LOG_INF("Start time: %d, Current time: %d", start_time, current_time);
		char buffer[32];
		sprintf(buffer, "Spel start in %d min", start_time > current_time ? start_time - current_time : start_time + (1440 - current_time));
		lcdStringWrite(buffer);
	}

	end_time = sd_get_end_time();
#endif
	LOG_INF("Going to idle\n");
	*next_state = idle_state;
}

void idle_stateFunction(statemachineStates* next_state, int* mgID, uint8_t* trivia_ID) {
	LOG_INF("Idle\n");
	char **names;
	unsigned amount;
	getIdleThreads(&names, &amount);
	enableThreads(names, amount);
	int ret = playIdle(trivia_ID);
	disableThreads(names, amount);

	if (ret < -1) {
		LOG_ERR("Error in idle state\n");
		*next_state = 0;
	} else if (ret == -1) {
		LOG_INF("Going to exit state\n");
		*next_state = exit_state;
	}else if(ret == 0){ 
		*next_state = trivia_state;
	} else {
		*next_state = mg_state;
		*mgID = ret;
	}
}

void mg_stateFunction(statemachineStates* next_state, int mgID) {
	LOG_INF("Minigame state\n");
	int score = 0;
	bool replayUsed = false;

	char **names = NULL;
	unsigned amount = 0;
	getMgThreads(&names, &amount, mgID);
	enableThreads(names, amount);

		switch(mgID){
		case 1:
            score = playMg1();
            break;
        case 2:
            score = playMg2();
            break;
        case 3:
			score = playMg3();
			break;
		case 4:
			score = playMg4();
			break;
		case 5:
			score = playMg5();
			break;
		case 6:
			score = playMg6();
			break;
		case 7:
			score = playMg7();
			break;
		case 8:
			score = playMg8();
			break;
		case 9:
			score = playMg9();
			break;
		case 10:
			score = playMg10();
			break;
		case 11:
			score = playMg11();
			break;
		case 12:
			score = playMg12();
			break;
		default:
			break;
    }

	disableThreads(names, amount);

	// replay the minigame if the score was below 500
	if((score < 500) && (firstTimeMG == true)){
		firstTimeMG = false;
		replayUsed = true;
		score = 0;
		lcdEnable();
		lcdStringWrite("Score was onder de 500");
		k_msleep(2000);
		lcdStringWrite("Spel wordt opnieuw gestart");
		k_msleep(2000);
		lcdDisable();
		mg_stateFunction(next_state, mgID);
		firstTimeMG = true;
	}

	if(!replayUsed){
		sd_add_score(score);
		show_mg_score(score);
	}

	*next_state = idle_state;
}

void trivia_stateFunction(statemachineStates* next_state, uint8_t trivia_ID) {
	LOG_INF("Trivia\n");
	int score = 0;

	char **names;
	unsigned amount;
	getTriviaThreads(&names, &amount);
	enableThreads(names, amount);

	score = playTrivia(trivia_ID);

	disableThreads(names, amount);
	sd_add_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void exit_stateFunction(statemachineStates* next_state) {
	LOG_INF("End game state");

	char **names;
	unsigned amount;
	getEndGameThreads(&names, &amount);
	enableThreads(names, amount);

	playEndGame();

	disableThreads(names, amount);
}

bool check_end_time_reached() {
	// Get the current time
	int16_t hour = getHour();
	int16_t minute = getMinute();
	int16_t current_time = (int16_t)((hour * 60) + minute);

	// Check if the end time has been reached
	if (current_time >= end_time) {
		LOG_INF("End time reached: %d", current_time);
		return true;
	}
	return false;
}

void startStatemachine() {
	bool statemachine_ongoing = 1;
	statemachineStates current_state = init_state;
	int mgID = 0;
	uint8_t trivia_ID = 0;

	while(statemachine_ongoing){
#if !defined(CONFIG_TESTMODE) && !defined(CONFIG_NOTIMEMODE)
	if(check_end_time_reached()){
		current_state = end_game_state;
	}
#endif
		switch (current_state) {
			case init_state:
				init_stateFunction(&current_state);
				LOG_INF("Score after init: %d", sd_get_score());
				break;
			case idle_state:
				idle_stateFunction(&current_state, &mgID, &trivia_ID);
				break;
			case mg_state:
				mg_stateFunction(&current_state, mgID);
				break;
			case trivia_state:
				trivia_stateFunction(&current_state, trivia_ID);
				break;
			case exit_state:
				exit_stateFunction(&current_state);
				break;
			default:
				break;
			}
	}
}
