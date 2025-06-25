#ifndef MINIGAME5_H
#define MINIGAME5_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg5ThreadCount 3

void getMg5Threads(char ***, unsigned *);

uint16_t generate_fruit_mg5();
int playMg5();

#endif // MINIGAME5_H