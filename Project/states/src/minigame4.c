#include "minigame4.h"


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
	"Trivia!",
	"Vragen over     Den Bosch",
	"& Vragen over   Avans"
};

int trivia_load(uint16_t type, struct Quiz **questions, size_t *count, size_t maxQuestions)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    char json_buf[BUFFER_SIZE];
    size_t len = 0;
	size_t i = 0;
    int ret = sd_get_buffer(type, json_buf, &len, BUFFER_SIZE);
    if (ret != 0){
        return ret;
	}

	if (len >= BUFFER_SIZE) {
    LOG_ERR("JSON too large for buffer");
    return -3;
	}
	json_buf[len] = '\0';  // Ensure null-termination

	LOG_WRN("%s",json_buf);
    struct Quiz *quizArray = malloc(maxQuestions * sizeof(struct Quiz));
    if (!quizArray){
		free(quizArray);
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

    
    for (; i < maxQuestions; ++i) {
        memset(&quizArray[i], 0, sizeof(struct Quiz));
        ret = json_arr_separate_parse_object(&json_arr, quiz_descr, ARRAY_SIZE(quiz_descr), &quizArray[i]);
        if (ret == 0) break; // End of array
        if (ret < 0) {
            LOG_ERR("Parse error at index %zu: %d", i, ret);
            // Free allocated quiz before returning
            free(quizArray);
            return ret;
        }
    }
	//LOG_WRN("%s", quizArray[1].question);
    *count = i;
    *questions = quizArray;
#endif
	//lcdStringWrite("Loaded Quiz");
	//k_msleep(3000);
    return 0;
}




int playMg4() {
	uint32_t score = 1000;
	uint8_t *abcBtn;
	bool showQuestion = true;
	bool correct = false;
	bool buttonReleased = true;
	static uint8_t questionIndex = 0;

	struct Quiz *quizzes = NULL;
	size_t count = 0;
	size_t maxQuestions = 32;
	char buf[64];
	lcdEnable();
	int res = trivia_load(3, &quizzes, &count, maxQuestions);
	if (res < 0) {
		sprintf(buf, "failed to load trivia: %d", res);
		LOG_INF("%s", buf);
		k_msleep(3000);
		return res;
	}

	
	sprintf(buf, "Total questions loaded =: %d ", count );
	LOG_INF(" %s", buf);
	

	show_oneliners(oneLinersMG4 , MG4_ONELINERS);
	lcdEnable();
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

			//show output
			if (showQuestion)
			{
				showQuestion = false;
				sprintf(buf, "%s ", quizzes[questionIndex].question);
		
				lcdStringWrite(buf);
				k_timer_start(&secTimerMg4, K_MSEC(1000), K_NO_WAIT);
				while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}

				char *answers[AMOUNT_ANSWERS] = { 
					quizzes[questionIndex].answerA, 
					quizzes[questionIndex].answerB, 
					quizzes[questionIndex].answerC
				};

				for (uint8_t answersIndex = 0; answersIndex < AMOUNT_ANSWERS; answersIndex++)
				{
					//sprintf(buf, "%s ",answers[answersIndex]);
					lcdStringWrite(answers[answersIndex]);
					k_timer_start(&secTimerMg4, K_MSEC(2000), K_NO_WAIT);
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
			
				if (!abcBtn[quizzes[questionIndex].correct])
				{
					lcdStringWrite("Correct!");
					correct = true;
				}
				else
				{
					lcdStringWrite("Incorrect!");
					score -= 400;
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
