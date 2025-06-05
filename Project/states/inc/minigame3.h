#ifndef MINIGAME3_H
#define MINIGAME3_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg3ThreadCount 4

void getMg3Threads(char ***, unsigned *);

int playMg3();

#endif // MINIGAME3_H