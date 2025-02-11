#include "minigame2.h"

K_TIMER_DEFINE(secTimerMg2, NULL, NULL);

char *mg2Threads[mg2ThreadCount] = {"startbtn", "btnmatrix_in", "btnmatrix_out", "buzzers"};

void getMg2Threads(char ***names, unsigned *amount) {
	*names = mg2Threads;
	*amount = mg2ThreadCount;
}

#define MG2_ONELINERS 4
char oneLinersMG2[MG2_ONELINERS][32] = {
	"Simon Says: Drukop de        ",
	"lichtgevende    knoppen      ",
	"in de juiste    volgorde     ",
	"Druk op de      startknop!   "
};


void generateSequence(uint8_t *sequence,uint8_t size,uint8_t bits) {
	for (uint8_t i = 0; i < size; i++)	{
			uint8_t randomValue = 0;
			sys_rand_get(&randomValue, sizeof(randomValue));
			randomValue = randomValue & (bits);
			sequence[i] = randomValue;
			printf_minigame2("random = sequence[%d]: %d\n",i,sequence[i]);
		}
}

void showButton(uint8_t btn)
{
	uint8_t data[4] = {0b00000000,0b00000000,0b00000000,0b00000000};
	switch (btn)
	{
	case 3:
	data[0] = 0b00001000;
		break;
	case 2:
	data[0] = 0b00000001;
		break;
	case 1:
	data[3] = 0b00001000;
		break;
	case 0:
	data[3] = 0b00000001;
		break;
	case 6:
	data[0] = 0b00001001;
	data[3] = 0b00001001;
		break;
	default:
		//should not reach
		break;
	}	
	btnmatrix_outSetMutexValue(data);
}

void showLevel(uint8_t level,uint8_t sequence[8]){
	for (uint8_t i = 0; i < level+1; i++)
	{
		showButton(sequence[i]);
		printf_minigame2("sequence[%d]: %d\n",i,sequence[i]);
		k_timer_start(&secTimerMg2, K_MSEC(1000), K_NO_WAIT);
		while (!(k_timer_status_get(&secTimerMg2) > 0)){}	
		showButton(5);
		k_timer_start(&secTimerMg2, K_MSEC(1000), K_NO_WAIT);
		while (!(k_timer_status_get(&secTimerMg2) > 0)){}	
	}
	showButton(6);
	k_timer_start(&secTimerMg2, K_MSEC(1000), K_NO_WAIT);
	while (!(k_timer_status_get(&secTimerMg2) > 0)){}	
	showButton(5);
}

uint8_t checkinput(bool* genValue,uint32_t* score, uint8_t level,uint8_t sequence[8])
{
	uint8_t *btnValues;
	uint8_t btnLevel = 0;
	bool buttonDetected = false;
	while(*genValue == true)
	{
		btnValues = btnmatrix_inGetMutexValue();
		if(btnValues[15] && btnValues[12] && btnValues[3] && btnValues[0])
		{
			buttonDetected = false;
			showButton(5);
			if(btnLevel == level+1)
			{
				return 1;
				break;
			}
		}
		if((!btnValues[15] || !btnValues[12] || !btnValues[3] || !btnValues[0]) && buttonDetected == false)
		{
			buttonDetected = true;
			printf_minigame2("sequence before %d \n",sequence[btnLevel]);
			switch (sequence[btnLevel])
			{
			case 3:
				if(btnValues[15] && btnValues[12] && btnValues[3] && !btnValues[0])
				{
					btnLevel++;
					printf_minigame2("3 correct \n");
					showButton(3);
				}
				else
				{
					*genValue = false;
					*score -= 100;
					printf_minigame2("3 incorrect \n");
					for (int i = 0; i < 16; i++)
					{
						printf_minigame2("button once %d: %d\n",i,btnValues[i]);
					}
				}
				break;
			case 2:
				if(btnValues[15] && btnValues[12] && !btnValues[3] && btnValues[0])
				{
					btnLevel++;
					printf_minigame2("2 correct \n");
					showButton(2);
				}
				else
				{
					*genValue = false;
					*score -= 100;
					printf_minigame2("2 incorrect \n");
					for (int i = 0; i < 16; i++)
					{
						printf_minigame2("button once %d: %d\n",i,btnValues[i]);
					}
				}
				break;
			case 1:
				if(btnValues[15] && !btnValues[12] && btnValues[3] && btnValues[0])
				{
					btnLevel++;
					printf_minigame2("1 correct \n");
					showButton(1);
				}
				else
				{
					*genValue = false;
					*score -= 100;
					printf_minigame2("1 incorrect \n");
					for (int i = 0; i < 16; i++)
					{
						printf_minigame2("button once %d: %d\n",i,btnValues[i]);
					}
				}
				break;
			case 0:
				if(!btnValues[15] && btnValues[12] && btnValues[3] && btnValues[0])
				{
					btnLevel++;
					printf_minigame2("0 correct \n");
					showButton(0);
				}
				else
				{
					*genValue = false;
					*score -= 100;
					printf_minigame2("0 incorrect \n");
					for (int i = 0; i < 16; i++)
					{
						printf_minigame2("button once %d: %d\n",i,btnValues[i]);
					}
				}
				break;
			}	
		}
	}
	return 0;
}

void showOnelinersMG2()
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
			for (uint8_t i = 0; i < MG2_ONELINERS; i++)
			{
			lcdStringWrite(oneLinersMG2[i]);
			k_timer_start(&secTimerMg2, K_MSEC(3000), K_NO_WAIT);
			while (!(k_timer_status_get(&secTimerMg2) > 0)){}	
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

int playMg2() {
	uint32_t score = 1000;
	uint8_t sequence[8] = {0,0,0,0,0,0,0,0};
	uint8_t level = 0;
	bool genValue = false;
	showOnelinersMG2();
	while (true)
	{
		if(genValue == false)
		{
			level = 0;
			generateSequence(sequence,8,0b11);
			genValue = true;
		}
		k_timer_start(&secTimerMg2, K_MSEC(1000), K_NO_WAIT);
		while (!(k_timer_status_get(&secTimerMg2) > 0)){}	
		showLevel(level,sequence);
		level += checkinput(&genValue, &score, level, sequence);
		if(level == 8 || score == 0)
		{
			break;
		}
	}
	return (int)score;
}
