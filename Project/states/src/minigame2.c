#include "minigame2.h"

LOG_MODULE_REGISTER(mg_2);

#define levels 11
#define maxAttempts 2 // Maximum number of attempts before the game ends
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

void buzzer0_off_callback(struct k_timer *timer_id)
{
	buzzerTurnOff(0);
}

K_TIMER_DEFINE(buzzer0_timer, buzzer0_off_callback, NULL);

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

/**
 * @brief Get input until the player makes a mistake, or the required input has been reached
 * @param sequence The sequence to check against
 * @param level The current level of the game, which determines how many inputs are expected
 * @return false if the player made a mistake, true if the input was correct
 */
bool get_input(uint8_t* sequence, uint8_t level){
	uint8_t input_sequence[levels] = {0};
	lcdStringWrite("Voer de juiste volgorde in!");

	for(int i = 0; i <= level; i++){
		input_sequence[i] = get_btnmatrix_input_number_untimed();

		buzzerSetPwm(0, noteSounds[input_sequence[i]] ? noteSounds[input_sequence[i]] : 0);
		k_timer_start(&buzzer0_timer, K_MSEC(2000), K_NO_WAIT); // Start the timer to turn off the buzzer after 2 seconds no matter if the button is pressed or not
		wait_till_btnmatrix_depressed();
		k_timer_stop(&buzzer0_timer); // Stop the timer if the button was released before the timer finished
		buzzerTurnOff(0);

		if(check_sequence(sequence, input_sequence, i) == 1){
			lcdClear();
			return false;
		}
	}
	lcdClear();
	return true;
}

int playMg2() {
	int8_t level = -1; // Start at level -1, so the first increment will be to level 0
	uint32_t score = 0;
	uint8_t sequence[levels] = {0};
	bool correct = true;
	uint8_t attempt = 0;

	show_oneliners(oneLinersMG2, MG2_ONELINERS);
	lcdEnable();
	
	generate_sequence(sequence);
	
	while (game_ongoing_mg2)
	{
		if(level < levels - 1){ // Check if the level is less than the maximum level first for less indents.
			level++;
			score = score + 100;
		}
		else{
			lcdStringWrite("gewonnen!");
			game_ongoing_mg2 = false;
			k_msleep(400);
		}

		native_loop();
		show_sequence(sequence, level);

		correct = get_input(sequence, level);
		k_msleep(200); // wait for a short time to allow a clear difference between user and system

		if(correct){
			lcdStringWrite("Correct!");
			buzzerSetPwm(0, 666); // Play a sound for correct input
			show_correct();
			buzzerTurnOff(0);
			lcdClear();
		} else {
			lcdStringWrite("Incorrect!");
			buzzerSetPwm(0, 82); // Play a sound for incorrect input
			show_incorrect();
			buzzerTurnOff(0);
			attempt++;
		}

		if (attempt > maxAttempts)
		{
			game_ongoing_mg2 = false;
		}
	}
	
	lcdClear();
	lcdDisable();

	clear_btnmatrix_leds();
	
	k_msleep(100);
	LOG_INF("Score is  %d\n", score );
	return attempt > 0 ? (int)score/attempt: (int)score; // Return the score divided by the number of attempts.
}
