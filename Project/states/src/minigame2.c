#include "minigame2.h"

LOG_MODULE_REGISTER(mg_2);

#define levels 11
bool game_ongoing_mg2 = 1;

K_TIMER_DEFINE(secTimerMg2, NULL, NULL);

char *mg2Threads[mg2ThreadCount] = {"startbtn", "btnmatrix_in", "btnmatrix_out", "buzzers"};

void getMg2Threads(char ***names, unsigned *amount) {
	*names = mg2Threads;
	*amount = mg2ThreadCount;
}

#define MG2_ONELINERS 3
char oneLinersMG2[MG2_ONELINERS][32] = {
	"Simon Says: Drukop de        ",
	"lichtgevende    knoppen      ",
	"in de juiste    volgorde     "
};

int noteSounds[16] = {
	// Frequencies for musical notes corresponding to the matrix
	220, 233, 249, 261, 277, 294, 330, 349, 392, 440, 466, 523, 554, 587, 659, 698
};

//returns 0 if sequence matches, 1 if no match
uint8_t check_sequence(uint8_t* sequence, uint8_t* input_sequence, uint8_t input)
{
	if (sequence[input] != input_sequence[input]) {
		LOG_INF("Sequences don't match\n");
		lcdStringWrite("Incorrect!");
		return 1;
	}

	return 0;
}

void generate_sequence(uint8_t* sequence) {
	LOG_INF("Sequence:");
	for(int i = 0; i < levels; i++){
		sequence[i] = sys_rand32_get() % 15;
		LOG_INF(" %d", sequence[i]);
	}
	LOG_INF("\n");
}

void show_sequence(uint8_t* sequence, uint8_t level)
{
	k_msleep(200); // wait for a short time to allow a clear difference between correct input and the sequence
	for(int i = 0; i <= level; i++){
		buzzerSetPwm(0, noteSounds[sequence[i]]);
		set_btnmatrix_led(sequence[i]);
		k_msleep(1500);
		clear_btnmatrix_leds();
		buzzerTurnOff(0);
		k_msleep(500); // separation between notes in the sequence
	}
}

//get input untill the player makes a mistake, or the required input has been reached
void get_input(uint8_t* sequence, uint8_t level){
	uint8_t input_sequence[levels] = {0};
	lcdStringWrite("Voer de juiste volgorde in!");

	for(int i = 0; i <= level; i++){
		input_sequence[i] = get_btnmatrix_input_number_untimed();

		buzzerSetPwm(0, noteSounds[input_sequence[i]] ? noteSounds[input_sequence[i]] : 0);
		wait_till_btnmatrix_depressed();
		buzzerTurnOff(0);

		if(check_sequence(sequence, input_sequence, i) == 1){
			k_msleep(200); // wait for a short time to allow a clear difference between user and system
			buzzerSetPwm(0, 82); // Play a sound for incorrect input
			show_incorrect();
			buzzerTurnOff(0);
			game_ongoing_mg2 = 0;
			break;
		}
	}
	lcdClear();
}

int playMg2() {
	uint8_t level = 0;
	uint32_t score = 0;
	uint8_t sequence[levels] = {0};

	show_oneliners(oneLinersMG2, MG2_ONELINERS);
	lcdEnable();
	
	generate_sequence(sequence);
	
	while (game_ongoing_mg2)
	{
		native_loop();
		show_sequence(sequence, level);
		get_input(sequence, level);

		if(game_ongoing_mg2){
			k_msleep(200); // wait for a short time to allow a clear difference between user and system
			lcdStringWrite("Correct!");
			buzzerSetPwm(0, 666); // Play a sound for correct input
			show_correct();
			buzzerTurnOff(0);
			lcdClear();
		}

		if(level < levels - 1){
			level++;
			score = score + 100;
		}
		else{
			lcdStringWrite("gewonnen!");
			game_ongoing_mg2 = false;
			k_msleep(400);
		}
	}
	
	lcdClear();
	lcdDisable();

	clear_btnmatrix_leds();
	
	k_msleep(100);
	LOG_INF("Score is  %d\n", score );
	return (int)score;
}
