#include "minigame11.h"

LOG_MODULE_REGISTER(mg_11);

void plate_timer_handler_mg11(struct k_timer *timer_id);
K_TIMER_DEFINE(plateTimer_mg11, plate_timer_handler_mg11, NULL);
K_TIMER_DEFINE(invaderTimer_mg11, NULL, NULL);
#define invader_timer_duration_mg11 150
#define hit_detection_row 14
#define plate_timer_duration_mg11 35
#define mg11_duration 15000
uint8_t plate_position_mg11 = 8;

uint16_t space_invader[6] = {
    0b0100000100000000,
    0b0011111000000000,
    0b0110101100000000,
    0b1111111110000000,
    0b1010001010000000,
    0b0001010000000000
};

char *mg11Threads[mg11ThreadCount] = {"startbtn", "ledmatrix", "abcbtn"};

void getMg11Threads(char ***names, unsigned *amount) {
	*names = mg11Threads;
	*amount = mg11ThreadCount;
}

#define MG11_ONELINERS 4
char oneLinersMG11[MG11_ONELINERS][32] = {
	"Space invaders!",
	"Gebruik de A en C knoppen om",
	"de plaat te bewegen",
    "Shiet met de B knop"
};

// Generates a new invader and prevents the generated invader from going outside bounds.
void generate_invader_mg11(uint16_t invader_masks[16]){
	//uint8_t invader_position = sys_rand32_get() % 10;

    for(int i = 0; i < 6; i++){
        invader_masks[i] |= space_invader[i];
    }
}

// Draws the game
void draw_game_mg11(uint16_t plate_mask, uint16_t invader_masks[16]){
	uint16_t game_frame[16] = {0};

	memcpy(game_frame, invader_masks, sizeof(game_frame));

	game_frame[15] = game_frame[15] | plate_mask;
	ledmatrixSetMutexValue(game_frame);
}

void plate_timer_handler_mg11(struct k_timer *timer_id){
	uint8_t *abcbtns;
	uint8_t *button = (uint8_t *)k_timer_user_data_get(&plateTimer_mg11);

	abcbtns = abcbtnGetMutexValue();

	if(abcbtns[*button] == 0){
		if((*button == 0) && (plate_position_mg11 < 14)){
			plate_position_mg11++;
		}
		else if((*button == 2) && (plate_position_mg11 > 1)){
			plate_position_mg11--;
		}
	}
}

void get_catch_input_mg11(){
	uint8_t *abcbtns;
	uint8_t button = 0;
	abcbtns = abcbtnGetMutexValue();

	if((abcbtns[0] == 0) && (abcbtns[2] == 0)){
		return;
	}

	if(abcbtns[0] == 0){
		if(k_timer_remaining_get(&plateTimer_mg11) == 0){
		k_sleep(K_MSEC(1));
		k_timer_user_data_set(&plateTimer_mg11, &button);
		k_timer_start(&plateTimer_mg11, K_MSEC(plate_timer_duration_mg11), K_NO_WAIT);
		}
		else{
			//timer already running
		}
	}

	if(abcbtns[2] == 0){
		if(k_timer_remaining_get(&plateTimer_mg11) == 0){
		button = 2;
		k_sleep(K_MSEC(1));
		k_timer_user_data_set(&plateTimer_mg11, &button);
		k_timer_start(&plateTimer_mg11, K_MSEC(plate_timer_duration_mg11), K_NO_WAIT);
		}
		else{
			//timer already running
		}
	}
}

int playMg11() {
	uint32_t score = 1000;
	uint16_t empty_frame[16] = {0};
	uint16_t plate_mask = {0};
	uint8_t invader_delay = 0;
	uint16_t invader_masks[16] = {0};
	uint8_t invaders_created = 0;
	char lcd_msg[32];

	memset(invader_masks, 0, sizeof(invader_masks));
	plate_position_mg11 = 8;

	show_oneliners(oneLinersMG11, MG11_ONELINERS);
	lcdEnable();
	wait_till_game_start();

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	lcdStringWrite("Score: 1000");
	k_timer_start(&invaderTimer_mg11, K_MSEC(invader_timer_duration_mg11), K_NO_WAIT);
	
	while (invaders_created < 20)
	{
		native_loop();
		// Handle invader related logic at a slower speed then the microcontroller
		if(k_timer_remaining_get(&invaderTimer_mg11) == 0){
			if(led_matrix_hit_detection(invader_masks, plate_mask, hit_detection_row)){
				score = score - 50;
				sprintf(lcd_msg, "Score: %d", score);
				lcdStringWrite(lcd_msg);
			}
			invader_delay++;
			if(invader_delay >= 6){
				invader_delay = 0;
				invaders_created++;
				generate_invader_mg11(invader_masks);
			}
			else{
				invader_masks[0] = 0b0000000000000000;
			}
			led_matrix_scroll_down(invader_masks);
			k_timer_start(&invaderTimer_mg11, K_MSEC(invader_timer_duration_mg11), K_NO_WAIT);
		}

		get_catch_input_mg11();
		plate_mask = (1 << plate_position_mg11) | (1 << (plate_position_mg11 - 1)) | (1 << (plate_position_mg11 + 1));
		draw_game_mg11(plate_mask, invader_masks);
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