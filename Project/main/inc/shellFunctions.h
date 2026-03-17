#ifndef SHELLFUNCTIONS_H
#define SHELLFUNCTIONS_H

#include <stdint.h>

struct location_new {
	int64_t lat;
	int64_t lon;
	uint8_t mg_id;
    uint8_t trivia_id;
};

// Compiler complains if you use a define for the char array size
struct trivia_question {
    uint8_t questionNumber;
	uint8_t correct;
    char question[50];
    char answerA[50];
    char answerB[50];
    char answerC[50];
};

#endif