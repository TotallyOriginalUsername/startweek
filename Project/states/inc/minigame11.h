#ifndef MINIGAME11_H
#define MINIGAME11_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg11ThreadCount 3

void getMg11Threads(char ***, unsigned *);

int playMg11();

#endif // MINIGAME11_H