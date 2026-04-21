#ifndef MINIGAME13_H
#define MINIGAME13_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg13ThreadCount 2

void getMg13Threads(char ***, unsigned *);

int playMg13();

#endif // MINIGAME13_H
