#include "minigame8.h"

K_TIMER_DEFINE(timerMg8, NULL, NULL);

char *mg8Threads[mg8ThreadCount] = {"startbtn", "btnmatrix_in", "btnmatrix_out"};

void getMg8Threads(char ***names, unsigned *amount) {
	*names = mg8Threads;
	*amount = mg8ThreadCount;
}

#define MG8_ONELINERS 3
char oneLinersMG8[MG8_ONELINERS][32] = {
	"10 seconden:   druk op een ",
	"lichtgevende knop als    ",
	"10 seconden voorbij zijn"
};

uint8_t get_any_btnmatrix(){
    uint8_t* input_array = btnmatrix_inGetMutexValue();

	for (int i = 0; i < 16; i++){
		if (input_array[i] == 0){
			return 0;
		}
	}
	return 1;
}

static void wait_till_game_start(){
	lcdStringWrite("Druk op start");
	startledSet(1);

	while(startbuttonGet()){
		native_loop();
	}
	startledSet(0);
}

int playMg8() {
	uint32_t score = 1000;
	uint32_t remaining_time = 0;
	uint8_t btnmatrix_on[4] = {0b11111111, 0b11111111, 0b11111111, 0b11111111};
	uint8_t btnmatrix_off[4] = {0};

	show_oneliners(oneLinersMG8, MG8_ONELINERS);
	lcdEnable();
	wait_till_game_start();
	lcdClear();
	lcdDisable();
	btnmatrix_outSetMutexValue(btnmatrix_on);

	k_timer_start(&timerMg8, K_MSEC(20000), K_NO_WAIT);
	
	while ((!(k_timer_status_get(&timerMg8) > 0)))
	{
		native_loop();
		if(get_any_btnmatrix() == 0){
			remaining_time = k_timer_remaining_get(&timerMg8);
			printk("Button pressed at: %d\n", remaining_time);
			break;
		}
	}

	if(remaining_time == 0){
		score = 0;
	}
	else if(remaining_time == 10000){
		score = 1000;
	}
	else if(remaining_time > 10000){
		score = score - (remaining_time - 10000)/10;
	}
	else{
		score = score - (10000 - remaining_time)/10;
	}
	printk("Score: %d\n", score);

	btnmatrix_outSetMutexValue(btnmatrix_off);
	k_msleep(100);
	
	return (int)score;
}
