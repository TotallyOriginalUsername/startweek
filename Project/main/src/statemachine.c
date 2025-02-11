// Includes: own header file, hardware headers, minigame headers, framework headers, system headers (sorted alpabetically)
#include "statemachine.h"
#include "threads.h"

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
#include <zephyr/sys/printk.h>

#include <stdio.h>

// Setup state machine
struct state;
typedef void state_fn(struct state *);

struct state {
	state_fn *next;
	int i;
};

state_fn init_state, idle_state, mg1_state, mg2_state, mg3_state, mg4_state, mg5_state, mg6_state, mg7_state, mg8_state, mg9_state, mg10_state, exit_state;
// Array of state functions
state_fn* minigame_states[] = {
    mg1_state, mg2_state, mg3_state, mg4_state, mg5_state,
    mg6_state, mg7_state, mg8_state, mg9_state, mg10_state
};

// State functions
void init_state(struct state *state) {
	printf("Initialization\n");
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
		printf("Error in idle state\n");
		state->next = 0;
	} else if (ret == -1) {
		printf("Going to exit state\n");
		state->next = exit_state;
	} else {
		state->next = minigame_states[ret];
	}
}

void mg1_state(struct state *state) { // Makes use of button and led
	// Initialise state, enable and disable corresponding threads
	printf("Minigame 1\n");

	char **names;
	unsigned amount;
	getMg1Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg1();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg2_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 2\n");

	char **names;
	unsigned amount;
	getMg2Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg2();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg3_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 3\n");

	char **names;
	unsigned amount;
	getMg3Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg3();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg4_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 4\n");

	char **names;
	unsigned amount;
	getMg4Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg4();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg5_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 5\n");

	char **names;
	unsigned amount;
	getMg5Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg5();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg6_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 6\n");

	char **names;
	unsigned amount;
	getMg6Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg6();

	disableThreads(names, amount);
	state->next = idle_state;
}

void mg7_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 7\n");

	char **names;
	unsigned amount;
	getMg7Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg7();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg8_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 8\n");

	char **names;
	unsigned amount;
	getMg8Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg8();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg9_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 9\n");

	char **names;
	unsigned amount;
	getMg9Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg9();

	disableThreads(names, amount);

	state->next = idle_state;
}

void mg10_state(struct state *state) { // Makes use of gyro and buzzer
	printf("Minigame 10\n");

	char **names;
	unsigned amount;
	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg10();

	disableThreads(names, amount);

	state->next = idle_state;
}

void exit_state(struct state *state) {
	printf("Exit state");
	disableAllThreads(); // Shouldn't be required, but just to be sure
	state->next = 0;
}

void startStatemachine() {
	struct state state = {init_state, 0};
	while (state.next) state.next(&state);
}