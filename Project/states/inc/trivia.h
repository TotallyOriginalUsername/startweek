#ifndef TRIVIA_H
#define TRIVIA_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include "helperFunctions.h"
#include "sdCard.h"
#include "shellFunctions.h"
#include <zephyr/data/json.h>

#define triviaThreadCount 3
#define AMOUNT_ANSWERS 3


void getTriviaThreads(char ***, unsigned *);

int playTrivia(uint8_t question_nr);

#endif // TRIVIA_H