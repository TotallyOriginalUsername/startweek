#ifndef MINIGAME10_H
#define MINIGAME10_H

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include "lcd.h"
#include "helperFunctions.h"

#define mg10ThreadCount 2

void getMg10Threads(char ***, unsigned *);

int playMg10();

#endif // MINIGAME10_H