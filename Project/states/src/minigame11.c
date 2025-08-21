#include "minigame11.h"

LOG_MODULE_REGISTER(mg_11);

void plate_timer_handler_mg11(struct k_timer *timer_id);
K_TIMER_DEFINE(bulletTimer_mg11, NULL, NULL);
K_TIMER_DEFINE(plateTimer_mg11, plate_timer_handler_mg11, NULL);
K_TIMER_DEFINE(invaderTimer_down_mg11, NULL, NULL);
K_TIMER_DEFINE(invaderTimer_horizontal_mg11, NULL, NULL);

#define bullet_timer_duration_mg11 50
#define invader_timer_down_duration_mg11 1500
#define invader_timer_horizontal_duration_mg11 250
#define hit_detection_row 14
#define plate_timer_duration_mg11 35
#define mg11_duration 15000

enum invader_directions {right, left};
struct bullets{
	uint8_t x;
	uint8_t y;
	bool exists;
};

uint8_t plate_position_mg11 = 8;

uint16_t space_invader[6] = {
    0b0000000000000000,
    0b0011001100110011,
	0b0000000000000000,
	0b0011001100110011,
	0b0000000000000000,
	0b0011001100110011
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

// Perform a hit detection if a bullet exists and clears the hit invader
// return 1 upon a hit and 0 upon no hit
bool bullet_invader_hit_detection(uint16_t invader_masks[16], struct bullets* bullet){
	uint16_t bitmask = 0;

	if(bullet->exists){
		bitmask = 1 << bullet->x;
		if(invader_masks[bullet->y] & bitmask){
			printk("Clearing invader and bullet\n");
			invader_masks[bullet->y] ^= bitmask;
			bullet->exists = false;
			return true;
		}
	}
	return false;
}

// Generates a new invader and prevents the generated invader from going outside bounds.
void generate_invader_mg11(uint16_t invader_masks[16]){
    for(int i = 0; i < 6; i++){
        invader_masks[i] |= space_invader[i];
    }
}

// Draws the game
void draw_game_mg11(uint16_t plate_mask, uint16_t invader_masks[16], struct bullets* bullet){
	uint16_t game_frame[16] = {0};
	uint16_t bitmask = 0;
	if(bullet->exists){
		bitmask = 1 << bullet->x;
	}

	memcpy(game_frame, invader_masks, sizeof(game_frame));
	game_frame[bullet->y] |= bitmask;

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

void get_input_mg11(struct bullets* bullet){
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

	if(abcbtns[1] == 0){
		if(bullet->exists){
			//printk("bullet already exists");
		}
		else{
			printk("Spawning new bullet\n");
			bullet->x = plate_position_mg11;
			bullet->y = 15;
			bullet->exists = true;
			k_timer_start(&bulletTimer_mg11, K_MSEC(bullet_timer_duration_mg11), K_NO_WAIT);
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

void update_bullet(struct bullets* bullet){
	if(bullet->exists){
		bullet->y = bullet->y - 1;
	}
	if(bullet->y <= 0){
		bullet->y = 17;
		bullet->exists = false;
	}
}

void update_invader_direction(uint16_t invader_masks[16], uint8_t* invader_direction){
	//use a static counter to keep track on how many times the invaders have moved, so that the direction
	//can be flipped before the invaders go out of bounds
	static uint8_t direction_count = 0;
	if(*invader_direction == left){
		for(int i = 0; i <16; i++){
			invader_masks[i] = invader_masks[i] << 1;
		}
	}
	else if(*invader_direction == right){
		for(int i = 0; i <16; i++){
			invader_masks[i] = invader_masks[i] >> 1;
		}
	}
	direction_count++;

	if(direction_count >= 2){
		if(*invader_direction == left){
			*invader_direction = right;
		}
		else{
			*invader_direction = left;
		}
		direction_count = 0;
	}
}

int playMg11() {
	uint32_t score = 0;
	uint16_t empty_frame[16] = {0};
	uint16_t plate_mask = {0};
	uint8_t invader_direction = left;
	uint16_t invader_masks[16] = {0};
	uint8_t game_ticks = 0;
	char lcd_msg[32];

	memset(invader_masks, 0, sizeof(invader_masks));
	plate_position_mg11 = 8;

	struct bullets bullet =	{17,17,false};

	//show_oneliners(oneLinersMG11, MG11_ONELINERS);
	lcdEnable();
	wait_till_game_start();

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	lcdStringWrite("Score: 0");
	k_timer_start(&invaderTimer_horizontal_mg11, K_MSEC(invader_timer_horizontal_duration_mg11), K_NO_WAIT);
	k_timer_start(&invaderTimer_down_mg11, K_MSEC(invader_timer_down_duration_mg11), K_NO_WAIT);
	generate_invader_mg11(invader_masks);
	
	while (game_ticks < 16)
	{
		native_loop();
		// Handle invader related logic at a slower speed then the microcontroller
		if(k_timer_remaining_get(&invaderTimer_horizontal_mg11) == 0){
			update_invader_direction(invader_masks, &invader_direction);
			k_timer_start(&invaderTimer_horizontal_mg11, K_MSEC(invader_timer_down_duration_mg11), K_NO_WAIT);
		}
		if(k_timer_remaining_get(&invaderTimer_down_mg11) == 0){
			if(led_matrix_hit_detection(invader_masks, plate_mask, hit_detection_row)){
				score = score - 50;
				sprintf(lcd_msg, "Score: %d", score);
				lcdStringWrite(lcd_msg);
			}
			led_matrix_scroll_down(invader_masks);
			k_timer_start(&invaderTimer_down_mg11, K_MSEC(invader_timer_down_duration_mg11), K_NO_WAIT);
			game_ticks++;
		}
		if(k_timer_remaining_get(&bulletTimer_mg11) == 0){
			update_bullet(&bullet);
			if(bullet_invader_hit_detection(invader_masks, &bullet)){
				printk("Bullet hit an invader\n");
				score = score + 50;
				sprintf(lcd_msg, "Score: %d", score);
				lcdStringWrite(lcd_msg);
			}
			if(bullet.exists){
				k_timer_start(&bulletTimer_mg11, K_MSEC(bullet_timer_duration_mg11), K_NO_WAIT);
			}
		}

		get_input_mg11(&bullet);
		plate_mask = (1 << plate_position_mg11) | (1 << (plate_position_mg11 - 1)) | (1 << (plate_position_mg11 + 1));
		draw_game_mg11(plate_mask, invader_masks, &bullet);
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
