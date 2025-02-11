#include "minigame3.h"
#include <zephyr/kernel.h>
#include <stdio.h>

char *mg3Threads[mg3ThreadCount] = {"startbtn", "btnmatrix_in", "btnmatrix_out", "buzzers"};

void getMg3Threads(char ***names, unsigned *amount) {
	*names = mg3Threads;
	*amount = mg3ThreadCount;
}

int playMg3() {
	// State loop
	for (int i = 0; i < 10; i++) {
		printf("Looping mg3, %d\n", i);
		k_msleep(10);
	}

	return 0;
}