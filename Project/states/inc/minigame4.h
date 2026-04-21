#ifndef MINIGAME4_H
#define MINIGAME4_H

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include "gps.h"
#include "buzzers.h"
#include "helperFunctions.h"
#include "sdCard.h"
#include "locations.h"
#include <stdlib.h>

#define mg4ThreadCount 6

void getMg4Threads(char ***, unsigned *);

int playMg4();

#endif // MINIGAME4_H