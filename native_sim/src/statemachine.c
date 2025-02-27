// Includes: own header file, hardware headers, minigame headers, framework headers, system headers (sorted alpabetically)
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include <stdio.h>
#include "statemachine.h"
#include "threads.h"

#include "idle.h"
#include "minigame2.h"


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
	printk("Initialization\n");
	disableAllThreads();

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
	printk("idle state\n");

	if (ret < -1 || ret > 9) {
		printk("Error in idle state\n");
		state->next = 0;
	} else if (ret == -1) {
		printk("Going to exit state\n");
		state->next = exit_state;
	} else {
		state->next = minigame_states[ret];
	}
	printk("idle state exit\n");
}

void mg1_state(struct state *state) { // Makes use of button and led
	// Initialise state, enable and disable corresponding threads
	printk("Minigame 1\n");

	char **names;
	unsigned amount;
/* 	getMg1Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg1();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg2_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 2\n");

	char **names;
	unsigned amount;
	getMg2Threads(&names, &amount);
	enableThreads(names, amount);
	printk("Minigame 2 threads enabled\n");

	int ret = playMg2();

	disableThreads(names, amount);
	printk("Minigame 2 threads disabled\n");

	state->next = idle_state;
}

void mg3_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 3\n");

	char **names;
	unsigned amount;
/* 	getMg3Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg3();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg4_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 4\n");

	char **names;
	unsigned amount;
/* 	getMg4Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg4();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg5_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 5\n");

	char **names;
	unsigned amount;
/* 	getMg5Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg5();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg6_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 6\n");

	char **names;
	unsigned amount;
	//getMg6Threads(&names, &amount);
	//enableThreads(names, amount);

	//int ret = playMg6();

	//disableThreads(names, amount);
	state->next = idle_state;
}

void mg7_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 7\n");

	char **names;
	unsigned amount;
/* 	getMg7Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg7();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg8_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 8\n");

	char **names;
	unsigned amount;
/* 	getMg8Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg8();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg9_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 9\n");

	char **names;
	unsigned amount;
/* 	getMg9Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg9();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void mg10_state(struct state *state) { // Makes use of gyro and buzzer
	printk("Minigame 10\n");

	char **names;
	unsigned amount;
/* 	getMg10Threads(&names, &amount);
	enableThreads(names, amount);

	int ret = playMg10();

	disableThreads(names, amount); */

	state->next = idle_state;
}

void exit_state(struct state *state) {
	printk("Exit state");
	disableAllThreads(); // Shouldn't be required, but just to be sure
	state->next = 0;
}

void startStatemachine() {
	struct state state = {init_state, 0};
	while (state.next) state.next(&state);
}