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

#define catchThePokemonThreadCount 6

#define BALLS 16
#define POKEMON 4
#define POKEMONLOCATIONS 4
#define MAX_ATTEMPTS 8
#define POKEMON_DISTANCE 10 // meters

void getCatchThePokemonThreads(char ***, unsigned *);

int playCatchThePokemon();

#endif // CATCHTHEPOKEMON_H