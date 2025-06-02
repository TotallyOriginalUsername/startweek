#include "minigame10.h"

K_TIMER_DEFINE(secTimerMg10, NULL, NULL);

char *mg10Threads[mg10ThreadCount] = {"btnmatrix_out", "btnmatrix_in"};
uint8_t mole_position = 0;
uint8_t mole_count = 0;
bool game_ongoing_mg10 = 1;

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
static void generate_mole(){
	uint32_t random_time = 0;
	random_time = (sys_rand32_get() % 2000) + 2000;
	printk("random time: %d\n", random_time);

	k_msleep(random_time);

	mole_position = sys_rand32_get() % 15;
	printk("random mole: %d\n", mole_position);

	set_btnmatrix_led(mole_position);
}

// Check if the input is correct
static void check_input(uint8_t* btnmatrix_in){
	uint8_t player_input = 0;
	uint8_t input_count = 0;

	for (int j = 0; j < 16; j++){
		if (btnmatrix_in[j] == 0){
			player_input = j;
			input_count++;
		}
	}

	if(input_count > 1){
		printk("Too many buttons pressed\n");
		lcdStringWrite("Probeer opnieuw!");
		return;
	}

	if(player_input == mole_position){
		mole_count++;
		printk("Mole pressed\n");
		lcdStringWrite("Mol gevangen!");
		if(mole_count == 10){
			game_ongoing_mg10 = 0;
		}
	}
	else{
		game_ongoing_mg10 = 0;
		lcdStringWrite("Mol ontsnapt!");
		printk("Mole escaped\n");
	}
}

// Wait till the user presses the button to start the game
static void wait_till_game_start(){
	lcdStringWrite("Druk op start");
	startledSet(1);

	while(startbuttonGet()){
		native_loop();
	}
	startledSet(0);
}

int playMg10() {
	uint32_t score = 0;
	uint8_t btnmatrix_in[16] = {1};
	uint16_t input_time = 5000;

	show_oneliners(oneLinersMG10, MG10_ONELINERS);

	lcdEnable();
	wait_till_game_start();
	
	while (game_ongoing_mg10)
	{
		native_loop();
		clear_btnmatrix_leds();
		lcdClear();
		generate_mole();
		get_btnmatrix_input(input_time, btnmatrix_in, sizeof(btnmatrix_in));
		check_input(btnmatrix_in);
	}
	score = mole_count * 100;
	printk("Score: %d\n", score);

	lcdClear();
	lcdDisable();
	
	return (int)score;
}
