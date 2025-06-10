#ifndef MINIGAME8_H
#define MINIGAME8_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "buttonMatrix.h"
#include "helperFunctions.h"
#include "threads.h"

#define mg8ThreadCount 3

uint8_t get_any_btnmatrix();
uint32_t calculate_score_mg8(uint32_t remaining_time);
void getMg8Threads(char ***, unsigned *);

int playMg8();

#endif // MINIGAME8_H