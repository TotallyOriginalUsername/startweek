#include "minigame9.h"

LOG_MODULE_REGISTER(mg_9);

#define max_rounds 5
#define display_time 5000

char *mg9Threads[mg9ThreadCount] = {"abcbtn"};
bool game_ongoing_mg9 = 1;

void getMg9Threads(char ***names, unsigned *amount) {
	*names = mg9Threads;
	*amount = mg9ThreadCount;
}

#define MG9_ONELINERS 4
char oneLinersMG9[MG9_ONELINERS][32] = {
	"Hoger of lager:",
	"Raadt of het volgende getal",
	"hoger of lager is! ",
	"abc links: lager,  rechts: hoger"
};

//return 0 if improper input, 1 if lower and 2 if higher
static uint8_t get_input(){
	uint16_t input_time = 5000;
	uint8_t abcbtns[3] = {1};

	wait_till_abc_depressed();
	abcledsSet('a', 1);
	abcledsSet('c', 1);

	get_abc_input(input_time, abcbtns, sizeof(abcbtns));

	if(abcbtns[0] && abcbtns[2] == 0){
		return 0;
	}

	if(abcbtns[0] == 0){
		return 1;
	}

	if(abcbtns[2] == 0){
		return 2;
	}

	return 0;
}

int playMg9() {
	uint32_t score = 0;
	uint8_t round = 0;
	uint8_t generated_number = 0;
	uint8_t next_generated_number = 0;
	uint8_t user_input = 0;
	char lcd_msg[32];

	show_oneliners(oneLinersMG9, MG9_ONELINERS);
	lcdEnable();

	//generate number, press lower(a) or higher(c), generate second number
	while (game_ongoing_mg9)
	{
		native_loop();;

		generated_number = sys_rand32_get() % 50;
		next_generated_number = sys_rand32_get() % 50;

		LOG_INF("Is het volgende getal %d hoger of lager dan %d\n", next_generated_number, generated_number);
		sprintf(lcd_msg, "Getal: %d Volgende getal is", generated_number);
		lcdStringWrite(lcd_msg);

		user_input = get_input();
		abcledsSet('a', 0);
		abcledsSet('c', 0);

		if(user_input == 0){
			lcdStringWrite("Druk a.u.b. een knop in");
			LOG_INF("Druk a.u.b. een knop in\n");
		}

		if((next_generated_number < generated_number) && (user_input == 1)){
			score = score + 200;
			lcdStringWrite("Correct!");
			LOG_INF("Correct!\n");
			k_msleep(5000);
		}
		else if((next_generated_number > generated_number) && (user_input == 2)){
			lcdStringWrite("Correct!");
			LOG_INF("Correct!\n");
			k_msleep(5000);
		}
		else{
			lcdStringWrite("Incorrect!");
			LOG_INF("Incorrect!\n");
			k_msleep(5000);
		}

		round++;

		if(round >= max_rounds){
			game_ongoing_mg9 = 0;
		}
	}

	lcdClear();
	lcdDisable();
	k_msleep(100);
	
	return (int)score;
}
