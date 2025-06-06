#include "minigame4.h"
#include "sdCard.h"
#include <zephyr/data/json.h>

#ifdef CONFIG_ARCH_POSIX
#define native_loop() k_sleep(K_MSEC(1))
#else
#define native_loop()
#endif
K_TIMER_DEFINE(secTimerMg4, NULL, NULL);
LOG_MODULE_REGISTER(mg_4);

char *mg4Threads[mg4ThreadCount] = {"startbtn", "buzzers", "abcbtn"};

void getMg4Threads(char ***names, unsigned *amount) {
	*names = mg4Threads;
	*amount = mg4ThreadCount;
}

#define MG4_ONELINERS 3
char oneLinersMG4[MG4_ONELINERS][32] = {
	"localee Trivia!",
	"Zephyr 3.6",
	"Z3.6 SDK 0.16.9"
};

char questions[AMOUNT_QUESTIONS][MAX_SIZE] = {
	"Hoe oud is Siem?",
	"Hoe groot is Siem?",
	"Waar werkt Siem?",
	"Waar woont Siem?"
};

char answers[AMOUNT_QUESTIONS][AMOUNT_ANSWERS][MAX_SIZE] = {
	{"A: 3 jaar", "B: 8 jaar", "C: 14 minuten"},
	{"A: 77cm", "B: 108cm", "C: 135cm"},
	{"A: Supermarkt", "B: Dierentuin", "C: School"},
	{"A: Limburg", "B: Gelderland", "C: Brabant"}
};

const int correctAnswer[AMOUNT_QUESTIONS] = {
	1,
	2,
	0,
	0,
};
/*
void showOnelinersMG4()
{
	bool done = false;
	lcdEnable();
	lcdStringWrite("Druk op start");
	while (!done)
	{
		native_loop();
		if(startbuttonGet())
		{	
			startledSet(1);
		}
		else
		{
			startledSet(0);
			for (uint8_t i = 0; i < MG4_ONELINERS; i++)
			{
			lcdStringWrite(oneLinersMG4[i]);
			k_timer_start(&secTimerMg4, K_MSEC(3000), K_NO_WAIT);
			while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}	
			}
			startledSet(1);
			while (true)
			{
				native_loop();
				if(!startbuttonGet())
				{	
				done = true;
				break;
				}
			}
			
		}
	}
	startledSet(0);
	//lcdClear();
	//lcdDisable();
}
*/


int trivia_load(uint16_t type, struct Quiz **questions, size_t *count, size_t maxQuestions)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    char json_buf[BUFFER_SIZE];
    size_t len = 0;
    int ret = sd_get_trivia(type, json_buf, &len, BUFFER_SIZE);
    if (ret != 0)
        return ret;

    struct Quiz *quizArray = malloc(sizeof(struct Quiz) * maxQuestions);
    if (!quizArray){
        return -2;
	}

	static const struct json_obj_descr quiz_descr[] = {
		JSON_OBJ_DESCR_PRIM(struct Quiz, question, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, answerA, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, answerB, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, answerC, JSON_TOK_STRING),
		JSON_OBJ_DESCR_PRIM(struct Quiz, correct, JSON_TOK_NUMBER),
	};


    struct json_obj json_arr;
    ret = json_arr_separate_object_parse_init(&json_arr, json_buf, len);
    if (ret < 0) {
        LOG_ERR("Parse init error: %d", ret);
        free(quizArray);
        return ret;
    }

    size_t i = 0;
    for (; i < maxQuestions; ++i) {
        memset(&quizArray[i], 0, sizeof(struct Quiz));
        ret = json_arr_separate_parse_object(&json_arr, quiz_descr, 2, &quizArray[i]);
        if (ret == 0) break; // End of array
        if (ret < 0) {
            LOG_ERR("Parse error at index %zu: %d", i, ret);
            // Free allocated quiz before returning
            free(quizArray);
            return ret;
        }
    }

    *count = i;
    *questions = quizArray;
	lcdStringWrite("Loaded QUIZEES");
	k_msleep(3000);
#endif
	lcdStringWrite("Loaded Quiz");
	k_msleep(3000);
    return 0;
}




int playMg4() {
	uint32_t score = 1000;
	uint8_t *abcBtn;
	bool showQuestion = true;
	bool correct = false;
	bool buttonReleased = true;
	static uint8_t questionIndex = 0;

	struct Quiz *locs = NULL;
	size_t count = 0;
	size_t maxQuestions = 32;
	char buf[64];
	lcdEnable();
	int res = trivia_load(3, &locs, &count, maxQuestions);
	if (res < 1) {
		sprintf(buf, "failed to load trivia: %d", res);
		lcdStringWrite(buf);
		k_msleep(3000);
		return -1;
	}

	

	for (size_t i = 0; i < count; ++i) {
		sprintf(buf, "%d: Q=%s cor= %d", i, locs[i].question , locs[i].correct);
		lcdStringWrite(buf);
		LOG_ERR("doing good things: %s", locs[1].question);
		k_msleep(3000);
	}


	show_oneliners(oneLinersMG4 , MG4_ONELINERS);
	k_timer_start(&secTimerMg4, K_MSEC(1000), K_NO_WAIT);
	abcledsSet('a',true);
	abcledsSet('b',true);
	abcledsSet('c',true);
	while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}	
	if( questionIndex < AMOUNT_QUESTIONS)
	{
		correct = false;
		while (!correct)
		{
			native_loop();
			//check if score is 0
			if(score == 0)
			{
				questionIndex = AMOUNT_QUESTIONS;
				break;
			}
			//show output
			if (showQuestion)
			{
				showQuestion = false;
				lcdStringWrite(questions[questionIndex]);
				k_timer_start(&secTimerMg4, K_MSEC(1000), K_NO_WAIT);
				while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}

				for (uint8_t answersIndex = 0; answersIndex < AMOUNT_ANSWERS; answersIndex++)
				{
					lcdStringWrite(answers[questionIndex][answersIndex]);
					k_timer_start(&secTimerMg4, K_MSEC(1000), K_NO_WAIT);
					while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}
				}
				
				lcdStringWrite("    Antwoord      A, B of C    ");
				
			}

			//check for input
			abcBtn = abcbtnGetMutexValue();
			if(abcBtn[2] && abcBtn[1] && abcBtn[0])
			{
				buttonReleased = true;
			}
			if ((!abcBtn[2] || !abcBtn[1] || !abcBtn[0]) && buttonReleased)
			{
				buttonReleased = false;
				switch (correctAnswer[questionIndex])
				{
				case 0:
					if (!abcBtn[0])
					{
						lcdStringWrite("Correct!");
						correct = true;
					}
					else
					{
						lcdStringWrite("Incorrect!");
						score -= 400;
					}
					break;
				case 1:
					if (!abcBtn[1])
					{
						lcdStringWrite("Correct!");
						correct = true;
					}
					else
					{
						lcdStringWrite("Incorrect!");
						score -= 400;
					}
					break;
				case 2:
					if (!abcBtn[2])
					{
						lcdStringWrite("Correct!");
						correct = true;
					}
					else
					{
						lcdStringWrite("Incorrect!");
						score -= 400;
					}
					break;
				default:
					break;
				}
				k_timer_start(&secTimerMg4, K_MSEC(1000), K_NO_WAIT);
				while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}	
				showQuestion = true;
			}
		}
	} 
	questionIndex++;
	abcledsSet('a',false);
	abcledsSet('b',false);
	abcledsSet('c',false);
	lcdDisable();
	lcdClear();
	if (score < 0){
		score = 0;
	}
	return score;
}
