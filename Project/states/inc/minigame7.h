#ifndef MINIGAME7_H
#define MINIGAME7_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include "helperFunctions.h"

#define mg7ThreadCount 3

void getMg7Threads(char ***, unsigned *);

uint16_t generate_fruit_mg7(uint16_t previous_fruit);
uint8_t update_fruit_mg7(uint16_t fruit_masks[16]);
bool hit_detection_mg7(uint16_t plate_mask, uint16_t fruit_masks[16]);
int playMg7();

#endif // MINIGAME1_H