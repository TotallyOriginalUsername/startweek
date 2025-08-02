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

int load_trivia(struct Quiz **question, uint8_t questionNr) {
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    char json_buf[BUFFER_SIZE];
	int ret = 0;
    size_t len = 0;
	struct json_obj json_arr;
	struct Quiz *quizArray = malloc(sizeof(struct Quiz));
	if (!quizArray){
		free(quizArray);
        return -2;
	}

	static const struct json_obj_descr quiz_descr[] = {
		JSON_OBJ_DESCR_PRIM(struct Quiz, question, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, answer0, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, answer1, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, answer2, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, correct, JSON_TOK_NUMBER),
	};

    ret = sd_get_trivia(questionNr, json_buf, &len, BUFFER_SIZE);
    if (ret != 0){
        return ret;
	}

	if (len >= BUFFER_SIZE) {
    	LOG_ERR("JSON too large for buffer");
    	return -3;
	}
	json_buf[len] = '\0';  // Ensure null-termination
	//LOG_WRN("%s",json_buf);

    ret = json_arr_separate_object_parse_init(&json_arr, json_buf, len);
    if (ret < 0) {
        LOG_ERR("Parse init error: %d", ret);
        free(quizArray);
        return ret;
    }

    ret = json_arr_separate_parse_object(&json_arr, quiz_descr, ARRAY_SIZE(quiz_descr), quizArray);
    if (ret < 0) {
        LOG_ERR("Parse error: %d", ret);
        free(quizArray);
        return ret;
    }

    *question = quizArray;
#endif
    return 0;
}

uint8_t show_trivia(struct Quiz *question){
	char *strings[4] = {question->question, question->answer0,question->answer1, question->answer2};

	for(int i = 0; i < 4; i++){
		lcdStringWrite(strings[i]);
		k_msleep(3000);
	}

	return 0;
}

int playTrivia(uint8_t question_nr) {
	uint32_t score = 0;
	uint8_t input = 0;
	struct Quiz *quiz = NULL;

	lcdEnable();
	int res = load_trivia(&quiz, question_nr);
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
