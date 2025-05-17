#include "minigame5.h"

void plate_timer_handler(struct k_timer *timer_id);
K_TIMER_DEFINE(plateTimer, plate_timer_handler, NULL);
K_TIMER_DEFINE(fruitTimer, NULL, NULL);
#define fruit_timer_duration 150
#define plate_timer_duration 35
#define mg5_duration 15000

uint16_t fruit_masks[16] = {0};
uint8_t plate_position = 8;
bool first_fruit = 1;

char *mg5Threads[mg5ThreadCount] = {"startbtn", "ledmatrix", "abcbtn"};

void getMg5Threads(char ***names, unsigned *amount) {
	*names = mg5Threads;
	*amount = mg5ThreadCount;
}

#define MG5_ONELINERS 3
char oneLinersMG5[MG5_ONELINERS][32] = {
	"Vang de rode ballen!",
	"Gebruik de A en C knoppen om",
	"de plaat te bewegen"
};

// Generates a new fruit and prevents the generated fruit from going outside bounds.
void generate_fruit(){

	fruit_masks[0] |= 1 << sys_rand32_get() % 16;
}

void update_fruit(){

	if(first_fruit){
		fruit_masks[0] = 0b0000000100000000;
		first_fruit = 0;
	}

	for(int i = 15; i > 0; i--){
		fruit_masks[i] = fruit_masks[i-1];
	}

}

// Draws the game
void draw_game(uint16_t plate_mask){
	uint16_t game_frame[16] = {0};

	memcpy(game_frame, fruit_masks, sizeof(game_frame));

	game_frame[15] = game_frame[15] | plate_mask;
	ledmatrixSetMutexValue(game_frame);
}

// Detects if a fruit properly hit the plate.
bool hit_detection(uint16_t plate_mask){

	if (fruit_masks[14] != 0)
	{
		if ((fruit_masks[14] & ~plate_mask) != 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

void plate_timer_handler(struct k_timer *timer_id){
	uint8_t *abcbtns;
	uint8_t *button = (uint8_t *)k_timer_user_data_get(&plateTimer);

	abcbtns = abcbtnGetMutexValue();

	if(abcbtns[*button] == 0){
		if((*button == 0) && (plate_position < 14)){
			plate_position++;
		}
		else if((*button == 2) && (plate_position > 1)){
			plate_position--;
		}
	}
}

void get_catch_input(){
	uint8_t *abcbtns;
	uint8_t button = 0;
	abcbtns = abcbtnGetMutexValue();

	if((abcbtns[0] == 0) && (abcbtns[2] == 0)){
		return;
	}

	if(abcbtns[0] == 0){
		if(k_timer_remaining_get(&plateTimer) == 0){
		k_sleep(K_MSEC(1));
		k_timer_user_data_set(&plateTimer, &button);
		k_timer_start(&plateTimer, K_MSEC(plate_timer_duration), K_NO_WAIT);
		}
		else{
			//timer already running
		}
	}

	if(abcbtns[2] == 0){
		if(k_timer_remaining_get(&plateTimer) == 0){
		button = 2;
		k_sleep(K_MSEC(1));
		k_timer_user_data_set(&plateTimer, &button);
		k_timer_start(&plateTimer, K_MSEC(plate_timer_duration), K_NO_WAIT);
		}
		else{
			//timer already running
		}
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
	lcdClear();
}

int playMg5() {
	uint32_t score = 1000;
	uint16_t empty_frame[16] = {0};
	uint16_t plate_mask = {0};
	uint8_t fruit_delay = 0;
	uint8_t fruits_created = 0;
	char lcd_msg[32];

	memset(fruit_masks, 0, sizeof(fruit_masks));
	plate_position = 8;
	first_fruit = 1;

	show_oneliners(oneLinersMG5, MG5_ONELINERS);
	lcdEnable();
	wait_till_game_start();

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	lcdStringWrite("Score: 1000");
	k_timer_start(&fruitTimer, K_MSEC(fruit_timer_duration), K_NO_WAIT);
	
	while (fruits_created < 20)
	{
		native_loop();
		// Handle fruit related logic at a slower speed then the microcontroller
		if(k_timer_remaining_get(&fruitTimer) == 0){
			if(hit_detection(plate_mask)){
				score = score - 50;
				sprintf(lcd_msg, "Score: %d", score);
				lcdStringWrite(lcd_msg);
			}
			fruit_delay++;
			if(fruit_delay >= 6){
				fruit_delay = 0;
				fruits_created++;
				generate_fruit();
			}
			else{
				fruit_masks[0] = 0b0000000000000000;
			}
			update_fruit();
			k_timer_start(&fruitTimer, K_MSEC(fruit_timer_duration), K_NO_WAIT);
		}

		get_catch_input();
		plate_mask = (1 << plate_position) | (1 << (plate_position - 1)) | (1 << (plate_position + 1));
		draw_game(plate_mask);
	}

	printk("Score: %d\n", score);
	abcledsSet('a', 0);
	abcledsSet('c', 0);
	lcdClear();
	lcdDisable();
	ledmatrixSetMutexValue(empty_frame);
	k_msleep(100);
	
	return (int)score;
}