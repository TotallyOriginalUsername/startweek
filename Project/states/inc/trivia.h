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
#include <zephyr/data/json.h>

#define triviaThreadCount 3
#define AMOUNT_ANSWERS 3
#define BUFFER_SIZE 2048 

struct Quiz {
	char* question;
	char* answer0;
	char* answer1;
	char* answer2;
	int correct;
};

void getTriviaThreads(char ***, unsigned *);

int load_trivia(struct Quiz **questions, uint8_t questionNr);
int playTrivia(uint8_t question_nr);

#endif // TRIVIA_H