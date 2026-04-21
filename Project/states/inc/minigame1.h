#ifndef MINIGAME1_H
#define MINIGAME1_H

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/random/random.h>
#include <string.h>
#include <zephyr/logging/log.h>
#include "helperFunctions.h"

#define mg1ThreadCount 3

struct apple_location{
	uint8_t x;
	uint8_t y;
};

enum snake_directions {up, right, down, left};
struct snake_locations{
	uint8_t x;
	uint8_t y;
};

void getMg1Threads(char ***, unsigned *);

void create_new_apple(struct apple_location* current_apple);
bool hit_detection_fruit(struct apple_location* current_apple, struct snake_locations* snake_mask);
bool hit_detection_snake(uint8_t snake_length, struct snake_locations* snake_mask);
void set_snake_direction(uint8_t btnValue, uint8_t* snake_direction);
int playMg1();

#endif // MINIGAME1_H