#ifndef MINIGAME10_H
#define MINIGAME10_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>
#include "lcd.h"
#include "helperFunctions.h"

#define mg10ThreadCount 2

void getMg10Threads(char ***, unsigned *);

uint8_t generate_mole();
bool check_input_mg10(uint8_t* btnmatrix_in, uint8_t mole_position);

int playMg10();

#endif // MINIGAME10_H