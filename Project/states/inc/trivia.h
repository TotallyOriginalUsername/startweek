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

void getTriviaThreads(char ***, unsigned *);

int playTrivia(uint8_t question_nr);
#define AMOUNT_QUESTIONS 4
#define AMOUNT_ANSWERS 3
// This buffer is related to the size of the trivia folder so this might need to be increased 
// asume 5-10 questions per 1000 bytes
// beware that this memory is used double, so increasing it eats the main stack quickly
#define BUFFER_SIZE 2048 

	struct Quiz {  //32 bytes per string max or it wont fit the display
		char* question;
		char* answerA;
		char* answerB;
		char* answerC;
		int correct;
	};


int trivia_load(uint16_t type, struct Quiz **questions, size_t *count, size_t maxQuestions);


#endif // TRIVIA_H