#ifndef MINIGAME12_H
#define MINIGAME12_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg12ThreadCount 3

void getMg12Threads(char ***, unsigned *);

int playMg12();

#endif // MINIGAME12_H
