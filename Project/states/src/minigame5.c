#include "minigame5.h"
#include <zephyr/kernel.h>
#include <stdio.h>

char *mg5Threads[mg5ThreadCount] = {"startbtn", "btnmatrix_in", "btnmatrix_out"};

void getMg5Threads(char ***names, unsigned *amount) {
	*names = mg5Threads;
	*amount = mg5ThreadCount;
}

int playMg5() {
	// State loop
	for (int i = 0; i < 10; i++) {
		printf("Looping mg5, %d\n", i);
		k_msleep(10);
	}

	return 0;
}