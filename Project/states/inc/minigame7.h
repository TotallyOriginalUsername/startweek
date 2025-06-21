#ifndef MINIGAME7_H
#define MINIGAME7_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg7ThreadCount 3

void getMg7Threads(char ***, unsigned *);

uint16_t generate_fruit_mg7(uint16_t previous_fruit);
int playMg7();

#endif // MINIGAME1_H