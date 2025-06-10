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

bool check_input_mg9(uint8_t generated_number, uint8_t next_generated_number, uint8_t user_input);
uint8_t get_input_mg9();
int playMg9();

#endif // MINIGAME9_H