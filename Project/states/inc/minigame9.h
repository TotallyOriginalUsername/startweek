#ifndef MINIGAME9_H
#define MINIGAME9_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>
#include "genericGpio.h"
#include "helperFunctions.h"
#include "lcd.h"

#define mg9ThreadCount 1

void getMg9Threads(char ***, unsigned *);

int playMg9();

#endif // MINIGAME9_H