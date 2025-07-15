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

enum statemachineStates {init_state, idle_state, end_game_state, exit_state, mg1_state, mg2_state, mg3_state, mg4_state,
						mg5_state, mg6_state, mg7_state, mg8_state, mg9_state, mg10_state, ctp_state};

enum statemachineStates current_state = init_state;
static int16_t end_time;

// State functions
enum statemachineStates init_stateFunction() {
	LOG_INF("Initialization\n");
	enum statemachineStates next_state;
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
			next_state = exit_state;
			return next_state;
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
	next_state = idle_state;
	return next_state;
}

enum statemachineStates idle_stateFunction() {
	LOG_INF("Idle\n");
	enum statemachineStates next_state;
	char **names;
	unsigned amount;
	getIdleThreads(&names, &amount);
	enableThreads(names, amount);
	int ret = playIdle();
	disableThreads(names, amount);

	if (ret < -1) {
		LOG_ERR("Error in idle state\n");
		next_state = 0;
	} else if (ret == -1) {
		LOG_INF("Going to exit state\n");
		next_state = exit_state;
	}else if(ret >= 100){   		// if a minigame ID above 100 is assigned, it is a triva question, (change this when more than 100 games are made)
		trivia_ID = ret - 100;		// internaly to the trivia game questions are labeled 0 to [however many are on the SD]
		next_state = mg4_state;	// keep in mind that increasing the amount of questions will influence required buffersizes, as well as the main stack
	} else {
		next_state = ret + MINIGAME_OFFSET;
	}
	return next_state;
}

enum statemachineStates mg1_stateFunction() { // Makes use of button and led
	// Initialise state, enable and disable corresponding threads
	LOG_INF("Minigame 1\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg1Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg1();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg2_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 2\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg2Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg2();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg3_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 3\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg3Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg3();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg4_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 4\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg4Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg4(trivia_ID);

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg5_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 5\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg5Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg5();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg6_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 6\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg6Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg6();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg7_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 7\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg7Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg7();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg8_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 8\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg8Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg8();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg9_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 9\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg9Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg9();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates mg10_stateFunction() { // Makes use of gyro and buzzer
	LOG_INF("Minigame 10\n");
	enum statemachineStates next_state;
	int score = 0;

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg10();

	disableThreads(names, amount);
	sd_set_score(score);
	show_mg_score(score);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates ctp_stateFunction() // Catch the Pokemon minigame
{
	char **names;
	unsigned amount;
	getCatchThePokemonThreads(&names, &amount);
	enableThreads(names, amount);
	enum statemachineStates next_state;

	int ret = playCatchThePokemon();

	if ( ret < 0 )
	{
		LOG_ERR("Error in catch the pokemon state\n");
		next_state = exit_state;
		return next_state;
	}

	disableThreads(names, amount);
	sd_set_score(ret);
	show_mg_score(ret);

	next_state = idle_state;
	return next_state;
}

enum statemachineStates end_game_stateFunction()
{
	LOG_INF("End game state");
	enum statemachineStates next_state;

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	playEndGame();

	disableThreads(names, amount);

	next_state = exit_state;
	return next_state;
}

enum statemachineStates exit_stateFunction() {
	LOG_INF("Exit state");
	enum statemachineStates next_state;
	disableAllThreads(); // Shouldn't be required, but just to be sure
	next_state = exit_state;
	return next_state;
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
	bool test = 0;

	while(statemachine_ongoing){
#ifndef CONFIG_TESTMODE
	statemachine_ongoing = check_end_time_reached();
#endif
#if defined(CONFIG_TESTMODE)
	if(test){
		LOG_INF("Time reached");
		statemachine_ongoing = 0;
	}
	else{
		LOG_INF("Time will be reached");
		statemachine_ongoing = 0;
	}
#endif
		switch (current_state) {
			case init_state:
				current_state = init_stateFunction();
				LOG_INF("Score after init: %d", sd_get_score());
				break;
			case idle_state:
				current_state = idle_stateFunction();
				break;
			case mg1_state:
				current_state = mg1_stateFunction();
				break;
			case mg2_state:
				current_state = mg2_stateFunction();
				break;
			case mg3_state:
				current_state = mg3_stateFunction();
				break;
			case mg4_state:
				current_state = mg4_stateFunction();
				break;
			case mg5_state:
				current_state = mg5_stateFunction();
				break;
			case mg6_state:
				current_state = mg6_stateFunction();
				break;
			case mg7_state:
				current_state = mg7_stateFunction();
				break;
			case mg8_state:
				current_state = mg8_stateFunction();
				break;
			case mg9_state:
				current_state = mg9_stateFunction();
				break;
			case mg10_state:
				current_state = mg10_stateFunction();
				break;
			case ctp_state:
				current_state = ctp_stateFunction();
				break;
			case end_game_state:
				current_state = end_game_stateFunction();
				break;
			case exit_state:
				current_state = exit_stateFunction();
				break;
			default:
				break;
			}
	}

	LOG_INF("Score after 0 games: %d", sd_get_score());

	current_state = end_game_stateFunction();
	while(1){
		current_state = exit_stateFunction();
	}
}