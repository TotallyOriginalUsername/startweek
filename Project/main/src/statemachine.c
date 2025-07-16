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
#include "catchThePokemon.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include <stdio.h>

LOG_MODULE_REGISTER(statemachine);
#define MINIGAME_OFFSET 4
// Setup state machine
uint8_t trivia_ID;

typedef enum {init_state, idle_state, end_game_state, exit_state, mg1_state, mg2_state, mg3_state, mg4_state,
						mg5_state, mg6_state, mg7_state, mg8_state, mg9_state, mg10_state, ctp_state} statemachineStates;
static int16_t end_time;

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
#ifndef CONFIG_TESTMODE
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

void idle_stateFunction(statemachineStates* next_state) {
	LOG_INF("Idle\n");
	char **names;
	unsigned amount;
	getIdleThreads(&names, &amount);
	enableThreads(names, amount);
	int ret = playIdle();
	disableThreads(names, amount);

	if (ret < -1) {
		LOG_ERR("Error in idle state\n");
		*next_state = 0;
	} else if (ret == -1) {
		LOG_INF("Going to exit state\n");
		*next_state = exit_state;
	}else if(ret >= 100){   		// if a minigame ID above 100 is assigned, it is a triva question, (change this when more than 100 games are made)
		trivia_ID = ret - 100;		// internaly to the trivia game questions are labeled 0 to [however many are on the SD]
		*next_state = mg4_state;	// keep in mind that increasing the amount of questions will influence required buffersizes, as well as the main stack
	} else {
		*next_state = ret + MINIGAME_OFFSET;
	}
}

void mg1_stateFunction(statemachineStates* next_state) { // Makes use of button and led
	// Initialise state, enable and disable corresponding threads
	LOG_INF("Minigame 1\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg1Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg1();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg2_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 2\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg2Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg2();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg3_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 3\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg3Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg3();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg4_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 4\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg4Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg4(trivia_ID);

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg5_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 5\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg5Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg5();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg6_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 6\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg6Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg6();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg7_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 7\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg7Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg7();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg8_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 8\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg8Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg8();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg9_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 9\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg9Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg9();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void mg10_stateFunction(statemachineStates* next_state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 10\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg10();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	*next_state = idle_state;
}

void ctp_stateFunction(statemachineStates* next_state) // Catch the Pokemon minigame
{
	char **names;
	unsigned amount;
	getCatchThePokemonThreads(&names, &amount);
	enableThreads(names, amount);

	int ret = playCatchThePokemon();

	if ( ret < 0 )
	{
		LOG_ERR("Error in catch the pokemon state\n");
		*next_state = exit_state;
	}

	disableThreads(names, amount);
	sd_set_score(ret);
	show_mg_score(ret);

	*next_state = idle_state;
}

void end_game_stateFunction(statemachineStates* next_state)
{
	LOG_INF("End game state");

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	playEndGame();

	disableThreads(names, amount);

	*next_state = exit_state;
}

void exit_stateFunction(statemachineStates* next_state) {
	LOG_INF("Exit state");
	disableAllThreads(); // Shouldn't be required, but just to be sure
	*next_state = exit_state;
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
	//bool test = 0;
	statemachineStates current_state = init_state;

	while(statemachine_ongoing){
// #ifndef CONFIG_TESTMODE
// 	statemachine_ongoing = check_end_time_reached();
	// if(check_end_time_reached()){
	// 	current_state = end_game_state;
	// }
// #endif
// #if defined(CONFIG_TESTMODE)
// 	if(test){
// 		LOG_INF("Time reached");
// 		statemachine_ongoing = 0;
// 	}
// 	else{
// 		LOG_INF("Time will be reached");
// 		statemachine_ongoing = 0;
// 	}
// #endif
		switch (current_state) {
			case init_state:
				init_stateFunction(&current_state);
				LOG_INF("Score after init: %d", sd_get_score());
				break;
			case idle_state:
				idle_stateFunction(&current_state);
				break;
			case mg1_state:
				mg1_stateFunction(&current_state);
				break;
			case mg2_state:
				mg2_stateFunction(&current_state);
				break;
			case mg3_state:
				mg3_stateFunction(&current_state);
				break;
			case mg4_state:
				mg4_stateFunction(&current_state);
				break;
			case mg5_state:
				mg5_stateFunction(&current_state);
				break;
			case mg6_state:
				mg6_stateFunction(&current_state);
				break;
			case mg7_state:
				mg7_stateFunction(&current_state);
				break;
			case mg8_state:
				mg8_stateFunction(&current_state);
				break;
			case mg9_state:
				mg9_stateFunction(&current_state);
				break;
			case mg10_state:
				mg10_stateFunction(&current_state);
				break;
			case ctp_state:
				ctp_stateFunction(&current_state);
				break;
			case end_game_state:
				end_game_stateFunction(&current_state);
				break;
			case exit_state:
				exit_stateFunction(&current_state);
				break;
			default:
				break;
			}
	}

	LOG_INF("Score after 0 games: %d", sd_get_score());

	end_game_stateFunction(&current_state);
	while(1){
		exit_stateFunction(&current_state);
	}
}