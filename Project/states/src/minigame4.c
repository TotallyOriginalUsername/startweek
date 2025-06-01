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

#define MG4_ONELINERS 2
char oneLinersMG4[MG4_ONELINERS][32] = {
	"Zephyr 3.6",
	"Z3.6 SDK 0.16.9"
};

char questions[AMOUNT_QUESTIONS][MAX_SIZE] = {
	"Hoe oud is Siem?",
	"Hoe groot is    Siem?",
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


int playMg4() {
	uint32_t score = 1000;
	uint8_t *abcBtn;
	bool showQuestion = true;
	bool correct = false;
	bool buttonReleased = true;
	showOnelinersMG4();
	k_timer_start(&secTimerMg4, K_MSEC(1000), K_NO_WAIT);
	abcledsSet('a',true);
	abcledsSet('b',true);
	abcledsSet('c',true);
	while (!(k_timer_status_get(&secTimerMg4) > 0)){native_loop();}	
	for (uint8_t questionIndex = 0; questionIndex < AMOUNT_QUESTIONS; questionIndex++)
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
						score -= 100;
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
						score -= 100;
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
						score -= 100;
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
	abcledsSet('a',false);
	abcledsSet('b',false);
	abcledsSet('c',false);
	lcdDisable();
	lcdClear();
	return score;
}
