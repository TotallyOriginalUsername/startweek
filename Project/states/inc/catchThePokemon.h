#ifndef CATCHTHEPOKEMON_H
#define CATCHTHEPOKEMON_H

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include "gps.h"
#include "buzzers.h"
#include "helperFunctions.h"
#include "sdCard.h"
#include "locations.h"
#include <stdlib.h>

#define catchThePokemonThreadCount 6

void getCatchThePokemonThreads(char ***, unsigned *);

int playCatchThePokemon();

#endif // CATCHTHEPOKEMON_H