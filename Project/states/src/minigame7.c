#include "minigame7.h"
#include <zephyr/kernel.h>
#include <stdio.h>

char *mg7Threads[mg7ThreadCount] = {"startbtn", "ledmatrix"};

void getMg7Threads(char ***names, unsigned *amount) {
	*names = mg7Threads;
	*amount = mg7ThreadCount;
}

int playMg7() {
	// State loop
	for (int i = 0; i < 10; i++) {
		printf("Looping mg7, %d\n", i);
		k_msleep(10);
	}

	return 0;
}