#include "minigame8.h"
#include <zephyr/kernel.h>
#include <stdio.h>

char *mg8Threads[mg8ThreadCount] = {"startbtn", "ledmatrix", "abcbtn"};

void getMg8Threads(char ***names, unsigned *amount) {
	*names = mg8Threads;
	*amount = mg8ThreadCount;
}

int playMg8() {
	// State loop
	for (int i = 0; i < 10; i++) {
		printf("Looping mg8, %d\n", i);
		k_msleep(10);
	}

	return 0;
}