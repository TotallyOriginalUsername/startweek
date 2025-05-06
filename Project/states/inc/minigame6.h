#ifndef MINIGAME6_H
#define MINIGAME6_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"

#define mg6ThreadCount 3

void getMg6Threads(char ***, unsigned *);

int playMg6();

#endif // MINIGAME6_H