#include "minigame13.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include <stdint.h>

LOG_MODULE_REGISTER(mg_13);

#define max_rounds 5

char *mg13Threads[mg13ThreadCount] = {"abcbtn", "switches"};

void getMg13Threads(char ***names, unsigned *amount) {
	*names = mg13Threads;
	*amount = mg13ThreadCount;
}

#define MG13_ONELINERS 3
char oneLinersMG13[MG13_ONELINERS][32] = {
	"Binary switches:",
	"Input het getal met de switches",
	"Hoog: 1         Laag: 0"
};

bool check_input_mg13(uint8_t generated_number){
	uint8_t* user_input = {0};
	uint8_t converted_input = 0;

	user_input = switchesGetMutexValue();

	// user_input is MSB and needs to be converted to LSB
	for(int i = 0; i < 5; i++){
		if(user_input[i] == 1){
			converted_input = converted_input | (1 << (4 - i));
		}
	}

	if(generated_number == converted_input){
		return 1;
	}

	return 0;
}

int playMg13() {
	uint32_t score = 0;
	uint8_t round = 0;
	uint8_t generated_number = 0;
	char lcd_msg[32];

	show_oneliners(oneLinersMG13, MG13_ONELINERS);
	lcdEnable();
	wait_till_game_start();

	generated_number = sys_rand32_get() % 31 + 1;
	sprintf(lcd_msg, "Getal: %d", generated_number);
	lcdStringWrite(lcd_msg);

	while(round < max_rounds){
		native_loop();

		if(check_input_mg13(generated_number)){
			score = score + 200;
			lcdStringWrite("Correct!");
			k_msleep(4000);
			generated_number = sys_rand32_get() % 31 + 1;
			sprintf(lcd_msg, "Getal: %d", generated_number);
			lcdStringWrite(lcd_msg);
			round++;
		}
	}

	LOG_INF("Score: %d\n", score);
	lcdClear();
	lcdDisable();
	k_msleep(50);
	
	return (int)score;
}
