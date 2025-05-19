#include "minigame1.h"
#include <zephyr/kernel.h>
#include <stdio.h>

char *mg1Threads[mg1ThreadCount] = {"startbtn", "ledmatrix"};

void getMg1Threads(char ***names, unsigned *amount) {
	*names = mg1Threads;
	*amount = mg1ThreadCount;
}

int playMg1() {
	// State loop
	for (int i = 0; i < 10; i++) {
		printf("Looping mg1, %d\n", i);
		k_msleep(10);
	}

	return 0;
}