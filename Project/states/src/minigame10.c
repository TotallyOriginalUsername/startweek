#include "minigame10.h"

LOG_MODULE_REGISTER(mg_10);
K_TIMER_DEFINE(secTimerMg10, NULL, NULL);

char *mg10Threads[mg10ThreadCount] = {"btnmatrix_out", "btnmatrix_in"};

void getMg10Threads(char ***names, unsigned *amount) {
	*names = mg10Threads;
	*amount = mg10ThreadCount;
}

#define MG10_ONELINERS 3
char oneLinersMG10[MG10_ONELINERS][32] = {
	"Whack a Mole:   druk op de ",
	"lichtgevende    knoppen    ",
	"op tijd     "
};

// Generate a random wait and then show a random mole
uint8_t generate_mole(){
	uint8_t mole_position = 0;
	uint32_t random_time = 0;
	random_time = (sys_rand32_get() % 2000) + 2000;
	LOG_INF("random time: %d\n", random_time);

	k_msleep(random_time);

	mole_position = sys_rand32_get() % 15;
	LOG_INF("random mole: %d\n", mole_position);

	return mole_position;
}

// Check if the input is correct
bool check_input_mg10(uint8_t* btnmatrix_in, uint8_t mole_position){
	uint8_t player_input = 16;
	uint8_t input_count = 0;

	for (int j = 0; j < 16; j++){
		if (btnmatrix_in[j] == 0){
			player_input = j;
			input_count++;
		}
	}
	LOG_INF("Player input: %d",player_input);

	if(input_count > 1){
		LOG_INF("Too many buttons pressed\n");
		lcdStringWrite("Probeer opnieuw!");
		return 0;
	}

	if(player_input == mole_position){
		LOG_INF("Mole pressed\n");
		lcdStringWrite("Mol gevangen!");
		return 1;
	}
	else{
		lcdStringWrite("Mol ontsnapt!");
		LOG_INF("Mole escaped\n");
	}

	return 0;
}

int playMg10() {
	uint32_t score = 0;
	uint8_t btnmatrix_in[16] = {1};
	uint8_t btnmatrix_off[4] = {0};
	uint16_t input_time = 5000;

	uint8_t mole_position = 0;
	uint8_t mole_caught = 0;
	uint8_t mole_count = 0;
	bool game_ongoing_mg10 = 1;

	show_oneliners(oneLinersMG10, MG10_ONELINERS);

	lcdEnable();
	wait_till_game_start();
	
	while (game_ongoing_mg10)
	{
		native_loop();
		clear_btnmatrix_leds();
		lcdClear();

		mole_position = generate_mole();
		set_btnmatrix_led(mole_position);
		mole_count++;

		get_btnmatrix_input(input_time, btnmatrix_in, sizeof(btnmatrix_in));
		if(check_input_mg10(btnmatrix_in, mole_position)){
			mole_caught++;
		}

		if(mole_count == 10){
			game_ongoing_mg10 = 0;
		}
	}
	score = mole_caught * 100;
	LOG_INF("Score: %d\n", score);

	lcdClear();
	lcdDisable();
	btnmatrix_outSetMutexValue(btnmatrix_off);
	k_msleep(100);
	
	return (int)score;
}
