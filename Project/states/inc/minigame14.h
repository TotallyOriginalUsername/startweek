#ifndef MINIGAME14_H
#define MINIGAME14_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg14ThreadCount 2

void getMg14Threads(char ***, unsigned *);

int playMg14();

#endif // MINIGAME14_H
