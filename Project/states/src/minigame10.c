#include "minigame10.h"
#include <zephyr/kernel.h>
#include <stdio.h>

char *mg10Threads[mg10ThreadCount] = {"startbtn", "btnmatrix_in", "switches", "potmeter", "abcbtn"};

void getMg10Threads(char ***names, unsigned *amount) {
	*names = mg10Threads;
	*amount = mg10ThreadCount;
}

int playMg10() {
	// State loop
	for (int i = 0; i < 10; i++) {
		printf("Looping mg10, %d\n", i);
		k_msleep(10);
	}

	return 0;
}