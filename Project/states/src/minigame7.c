#include "minigame7.h"

LOG_MODULE_REGISTER(mg_7);

void plate_timer_handler(struct k_timer *timer_id);
K_TIMER_DEFINE(plateTimer, plate_timer_handler, NULL);
K_TIMER_DEFINE(fruitTimer, NULL, NULL);
K_TIMER_DEFINE(mg7Timer, NULL, NULL);
#define fruit_timer_duration 150
#define plate_timer_duration 45
#define mg7_duration 15000
uint8_t plate_position = 8;

char *mg7Threads[mg7ThreadCount] = {"startbtn", "ledmatrix", "abcbtn"};

void getMg7Threads(char ***names, unsigned *amount) {
	*names = mg7Threads;
	*amount = mg7ThreadCount;
}

#define MG7_ONELINERS 3
char oneLinersMG7[MG7_ONELINERS][32] = {
	"Volg de rode lijn met de plaat!",
	"Gebruik de A en C knoppen om",
	"de plaat te bewegen"
};

// Generates a new fruit and prevents the generated fruit from going outside bounds.
uint16_t generate_fruit_mg7(uint16_t previous_fruit){

	if (sys_rand32_get() % 2 == 0) {
		//create a fruit to the right of previous fruit
		if( previous_fruit != 0b0000000000000001){
			return previous_fruit >>= 1;
		}
		else{
			return 0b0000000000000001;
		}
	}
	else {
		//create a fruit to the left of previous fruit
		if( previous_fruit != 0b1000000000000000){
			return previous_fruit <<= 1;
		}
		else{
			return 0b1000000000000000;
		}
	}
}

uint8_t update_fruit_mg7(uint16_t fruit_masks[16]){
	for(int i = 15; i > 0; i--){
		fruit_masks[i] = fruit_masks[i-1];
	}
	return 0;
}

// Draws the game
void draw_game_mg7(uint16_t plate_mask, uint16_t fruit_masks[16]){
	uint16_t game_frame[16] = {0};

	memcpy(game_frame, fruit_masks, sizeof(game_frame));

	game_frame[15] = game_frame[15] | plate_mask;
	ledmatrixSetMutexValue(game_frame);
}

// Detects if a fruit properly hit the plate.
bool hit_detection_mg7(uint16_t plate_mask, uint16_t fruit_masks[16]){

	if (fruit_masks[15] != 0)
	{
		if ((fruit_masks[15] & ~plate_mask) != 0)
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

int playMg7() {
	uint32_t score = 1000;
	uint16_t empty_frame[16] = {0};
	uint16_t plate_mask = {0};
	uint8_t fruit_delay = 0;
	uint16_t fruit_masks[16] = {0};
	char lcd_msg[32];
	plate_position = 8;

	memset(fruit_masks, 0, sizeof(fruit_masks));
	fruit_masks[0] = 0b0000000100000000;

	show_oneliners(oneLinersMG7, MG7_ONELINERS);
	lcdEnable();
	wait_till_game_start();

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	lcdStringWrite("Score: 1000");
	k_timer_start(&mg7Timer, K_MSEC(mg7_duration), K_NO_WAIT);
	k_timer_start(&fruitTimer, K_MSEC(fruit_timer_duration), K_NO_WAIT);
	
	while (!(k_timer_status_get(&mg7Timer) > 0))
	{
		native_loop();
		// Handle fruit related logic at a slower speed then the microcontroller
		if(k_timer_remaining_get(&fruitTimer) == 0){
			if(hit_detection_mg7(plate_mask, fruit_masks)){
				if(score >= 20){
					score = score - 20;
					sprintf(lcd_msg, "Score: %d", score);
					lcdStringWrite(lcd_msg);
				}
			}
			fruit_delay++;
			if(fruit_delay >= 2){
				fruit_delay = 0;
				fruit_masks[0] = generate_fruit_mg7(fruit_masks[0]);
			}
			update_fruit_mg7(fruit_masks);
			k_timer_start(&fruitTimer, K_MSEC(fruit_timer_duration), K_NO_WAIT);
		}

		get_catch_input();
		plate_mask = (1 << plate_position) | (1 << (plate_position - 1)) | (1 << (plate_position + 1));
		draw_game_mg7(plate_mask, fruit_masks);
	}

	LOG_INF("Score: %d\n", score);
	abcledsSet('a', 0);
	abcledsSet('c', 0);
	lcdClear();
	lcdDisable();
	ledmatrixSetMutexValue(empty_frame);
	k_msleep(100);
	
	return (int)score;
}