#ifndef CATCHTHEPOKEMON_H
#define CATCHTHEPOKEMON_H

#define DEBUG_MODE
#ifdef DEBUG_MODE
#define printf_catchThePokemon printf
#else
#define printf_catchThePokemon(...)
#endif //DEBUG_MODE

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include "gps.h"
#include "buzzers.h"

#define catchThePokemonThreadCount 4

#define BALLS 16
#define POKEMON 4
#define POKEMONLOCATIONS 4
#define MAX_ATTEMPTS 8

void getCatchThePokemonThreads(char ***, unsigned *);

int playCatchThePokemon();

#endif // CATCHTHEPOKEMON_H