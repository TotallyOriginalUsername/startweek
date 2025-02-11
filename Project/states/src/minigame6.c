#include "minigame6.h"

K_TIMER_DEFINE(secTimerMg6, NULL, NULL);


char *mg6Threads[mg6ThreadCount] = {"startbtn", "sevenseg", "abcbtn"};

void getMg6Threads(char ***names, unsigned *amount) {
	*names = mg6Threads;
	*amount = mg6ThreadCount;
}

#define MG6_ONELINERS 3
char oneLinersMG6[MG6_ONELINERS][32] = {
	"    Minigame          Timer",
	"Druk op de      verlichte knop",
	"Het dichtst bij de 0"
};

void showOnelinersMG6()
{
	bool done = false;
	lcdEnable();
	lcdStringWrite("Druk op start");
	while (!done)
	{
		if(startbuttonGet())
		{	
			startledSet(1);
		}
		else
		{
			startledSet(0);
			for (uint8_t i = 0; i < MG6_ONELINERS; i++)
			{
			lcdStringWrite(oneLinersMG6[i]);
			k_timer_start(&secTimerMg6, K_MSEC(3000), K_NO_WAIT);
			while (!(k_timer_status_get(&secTimerMg6) > 0)){}	
			}
			startledSet(1);
			while (true)
			{
				if(!startbuttonGet())
				{	
				done = true;
				break;
				}
			}
			
		}
	}
	startledSet(0);
	lcdClear();
	lcdDisable();
}



int playMg6() {
	uint32_t score = 1000;
	uint8_t *abcBtn;
	uint8_t value10 = 1;
	uint8_t value1 = 0;
	uint8_t value01 = 0;
	uint8_t value001 = 0;
	char input[4] = {"1000"};
	char clear[4] = {"0000"};

	showOnelinersMG6();
	abcledsSet('a', true);
	while(true)
	{
		abcBtn = abcbtnGetMutexValue();
		if (!abcBtn[0])
		{
			break;
		}
		input[0] = value10 + 48;
		input[1] = value1 + 48;
		input[2] = value01 + 48;
		input[3] = value001 + 48;
		sevensegSetMutexValue(input,1);
		k_timer_start(&secTimerMg6, K_MSEC(10), K_NO_WAIT);
		while (!(k_timer_status_get(&secTimerMg6) > 0)){}
		if(value001 == 0 && value01 == 0 && value1 == 0 && value10 == 0)
		{
			break;
		}
		if (value001 > 0) {
			value001--;
		} else {
			if (value01 > 0) {
				value01--;
				value001 = 9; //Set value001 to 9 if it was 0
			} else {
				if (value1 > 0) {
					value1--;
					value01 = 9; //Set value01 to 9 if it was 0
					value001 = 9; //Set value001 to 9 if both were 0
				} else {
					if (value10 > 0) {
						value10--;
						value1 = 9; //Set value1 to 9 if it was 0
						value01 = 9; //Set value01 to 9 if both were 0
						value001 = 9; //Set value001 to 9 if all were 0
					}
				}
			}
		}
	}
	abcledsSet('a', false);
	score = 1000 - ((value10 * 1000)+(value1 * 100)+(value01 * 10)+(value001 * 1));
	k_timer_start(&secTimerMg6, K_MSEC(3000), K_NO_WAIT);
	while (!(k_timer_status_get(&secTimerMg6) > 0)){}
	sevensegSetMutexValue(clear,0);
	return score;
}