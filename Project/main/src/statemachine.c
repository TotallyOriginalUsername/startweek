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

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include <stdio.h>

LOG_MODULE_REGISTER(statemachine);

// Setup state machine
struct state;
typedef void state_fn(struct state *);

struct state {
	state_fn *next;
	int i;
};

state_fn init_state, idle_state, mg1_state, mg2_state, mg3_state, mg4_state, mg5_state, mg6_state, mg7_state, mg8_state, mg9_state, mg10_state, end_game_state, exit_state;
// Array of state functions
state_fn* minigame_states[] = {
    mg1_state, mg2_state, mg3_state, mg4_state, mg5_state,
    mg6_state, mg7_state, mg8_state, mg9_state, mg10_state
};

static int16_t end_time;

// State functions
void init_state(struct state *state) {
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

	// check if current time is the same as the start time
	int16_t start_time = 0;
	int16_t current_time = 0;
	lcdEnable();
	while (current_time < start_time || current_time == 0)
	{
		start_time = sd_get_start_time();
		int16_t hour = getHour();
		int16_t minute = getMinute();
		current_time = (int16_t)((hour * 60) + minute);
		// LOG_INF("Start time: %d, Current time: %d", start_time, current_time);
		char buffer[32];
		sprintf(buffer, "Spel start in %d min", start_time < current_time ? start_time - current_time : start_time + (1440 - current_time));
		lcdStringWrite(buffer);
	}

	end_time = sd_get_end_time();

	state->next = idle_state;
}

void idle_state(struct state *state) {
	char **names;
	unsigned amount;
	getIdleThreads(&names, &amount);
	enableThreads(names, amount);
	int ret = playIdle();
	disableThreads(names, amount);

	if (ret < -1 || ret > 9) {
		LOG_ERR("Error in idle state\n");
		state->next = 0;
	} else if (ret == -1) {
		LOG_INF("Going to exit state\n");
		state->next = exit_state;
	} else {
		state->next = minigame_states[ret];
	}
}

void mg1_state(struct state *state) { // Makes use of button and led
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

	state->next = idle_state;
}

void mg2_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 2\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg2Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg2();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg3_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 3\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg3Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg3();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg4_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 4\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg4Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg4();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg5_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 5\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg5Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg5();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg6_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 6\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg6Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg6();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg7_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 7\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg7Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg7();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg8_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 8\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg8Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg8();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg9_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 9\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg9Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg9();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}

void mg10_state(struct state *state) { // Makes use of gyro and buzzer
	LOG_INF("Minigame 10\n");
	int score = 0;

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	score = playMg10();

	disableThreads(names, amount);
	sd_set_score(score);

	state->next = idle_state;
}


void end_game_state(struct state *state)
{
	LOG_INF("End game state");

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	playEndGame();

	disableThreads(names, amount);

	// state->next = exit_state;
}

void exit_state(struct state *state) {
	LOG_INF("Exit state");
	disableAllThreads(); // Shouldn't be required, but just to be sure
	state->next = 0;
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
	struct state state = {init_state, 0};
	while (state.next)
	{
		state.next(&state);
		if (check_end_time_reached())
			state.next = end_game_state;
	}
}