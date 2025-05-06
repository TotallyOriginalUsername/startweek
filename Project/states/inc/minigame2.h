#ifndef MINIGAME2_H
#define MINIGAME2_H

#define DEBUG_MODE
#ifdef DEBUG_MODE
#define printf_minigame2 printf
#else
#define printf_minigame2(...)
#endif //DEBUG_MODE

#include <zephyr/kernel.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include <zephyr/random/random.h>

#define mg2ThreadCount 4

void getMg2Threads(char ***, unsigned *);

int playMg2();

#endif // MINIGAME2_H