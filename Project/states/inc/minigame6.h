#ifndef MINIGAME6_H
#define MINIGAME6_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include "helperFunctions.h"
#include "sevenSegment.h"

#define mg6ThreadCount 2

void getMg6Threads(char ***, unsigned *);

int playMg6();

#endif // MINIGAME6_H