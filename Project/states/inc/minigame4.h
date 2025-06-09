#ifndef MINIGAME4_H
#define MINIGAME4_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "threads.h"
#include "lcd.h"
#include "genericGpio.h"
#include "helperFunctions.h"

#define mg4ThreadCount 3

void getMg4Threads(char ***, unsigned *);

int playMg4();
#define AMOUNT_QUESTIONS 4
#define AMOUNT_ANSWERS 3
#define BUFFER_SIZE 2048 // dirty to have this here.

	struct Quiz {  //this is dangerous
		char* question;
		char* answerA;
		char* answerB;
		char* answerC;
		int correct;
	};


int trivia_load(uint16_t type, struct Quiz **questions, size_t *count, size_t maxQuestions);


#endif // MINIGAME4_H