#include "trivia.h"

K_TIMER_DEFINE(secTimerTrivia, NULL, NULL);
LOG_MODULE_REGISTER(trivia);

char *triviaThreads[triviaThreadCount] = {"startbtn", "buzzers", "abcbtn"};

void getTriviaThreads(char ***names, unsigned *amount) {
	*names = triviaThreads;
	*amount = triviaThreadCount;
}

#define MG4_ONELINERS 3
char oneLinersMG4[MG4_ONELINERS][32] = {
	"Trivia!",
	"Vragen over     Den Bosch",
	"& Vragen over   Avans"
};

//return 3 if improper input, otherwise the pressed abc button
uint8_t get_input_trivia(){
	uint16_t input_time = 5000;
	uint8_t abcbtns[3] = {1};
	uint8_t input_count = 0;

	wait_till_abc_depressed();
	abcledsSet('a', 1);
	abcledsSet('c', 1);

	get_abc_input(input_time, abcbtns, sizeof(abcbtns));

	for (int i = 0; i < 3; i++){
		if (abcbtns[i] == 0){
			input_count++;
		}
	}

	if(input_count >= 2){
		return 3;
	}

	if(abcbtns[0] == 0){
		return 0;
	} else if(abcbtns[1] == 0){
		return 1;
	}else if(abcbtns[2] == 0){
		return 2;
	}

	return 3;
}

uint8_t show_trivia(struct trivia_question *question){
	char *strings[4] = {question->question, question->answerA,question->answerB, question->answerC};

	for(int i = 0; i < 4; i++){
		lcdStringWrite(strings[i]);
		k_msleep(3000);
	}

	return 0;
}

int playTrivia(uint8_t question_nr) {
	uint32_t score = 0;
	uint8_t input = 0;
	struct trivia_question* quiz = NULL;
	quiz->questionNumber = question_nr;

	lcdEnable();
	int res = sd_get_trivia_question(quiz);
	if (res < 0) {
		LOG_ERR("failed to load trivia: %d", res);
		return res;
	}
	
	show_oneliners(oneLinersMG4 , MG4_ONELINERS);
	lcdEnable();
	abcledsSet('a',true);
	abcledsSet('b',true);
	abcledsSet('c',true);

	show_trivia(quiz);
	lcdClear();
	input = get_input_trivia();

	if(input == 3){
		lcdStringWrite("Druk a.u.b. maar een knop in");
		k_msleep(3000);
	}
	if(input == quiz->correct){
		lcdStringWrite("Correct!");
		score = 750;
		k_msleep(3000);
	} else {
		lcdStringWrite("Incorrect!");
		k_msleep(3000);
	}

	abcledsSet('a',false);
	abcledsSet('b',false);
	abcledsSet('c',false);
	lcdClear();
	lcdDisable();

	return score;
}
