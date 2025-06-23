#ifndef MINIGAME1_H
#define MINIGAME1_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include <zephyr/logging/log.h>
#include "threads.h"
#include "hardware.h"
#include "helperFunctions.h"

#define mg1ThreadCount 3

void getMg1Threads(char ***, unsigned *);

int playMg1();

#endif // MINIGAME1_H