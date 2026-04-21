#ifndef MINIGAME15_H
#define MINIGAME15_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg15ThreadCount 2

void getMg15Threads(char ***, unsigned *);

int playMg15();

#endif // MINIGAME15_H
