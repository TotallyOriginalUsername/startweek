#include "minigame1.h"

K_TIMER_DEFINE(mg1Timer, NULL, NULL);
#define mg1Timer_duration 200
#define snake_max_length 15

bool button_pressed_mg1 = 0;
bool game_ongoing_mg1 = 1;

struct apple_location{
	uint8_t x;
	uint8_t y;
};
struct apple_location current_apple = {0, 0};

enum snake_directions {up, right, down, left};
struct snake_locations{
	uint8_t x;
	uint8_t y;
};
uint8_t snake_direction = up;
struct snake_locations snake_mask[snake_max_length] = {[0 ... snake_max_length - 1] = {17, 17}};
uint8_t snake_length = 3;

char *mg1Threads[mg1ThreadCount] = {"startbtn", "ledmatrix", "abcbtn"};

void getMg1Threads(char ***names, unsigned *amount) {
	*names = mg1Threads;
	*amount = mg1ThreadCount;
}

#define MG1_ONELINERS 3
char oneLinersMG1[MG1_ONELINERS][32] = {
	"Snake! Gebruikt de A(links)",
	"en C(rechts) knoppen",
	"om de slang te bewegen!"
};

//Returns the pressed button, or 17 if unproper or no input.
uint8_t get_abcbutton_input_number_untimed(){
	uint8_t input_array[3] = {1};
	uint8_t input_button = 0;
	uint8_t input_count = 0;

	memcpy(input_array, abcbtnGetMutexValue(), sizeof(input_array));

	for (int i = 0; i < 3; i++){
		if (input_array[i] == 0){
			input_button = i;
			input_count++;
		}
	}

	if(input_count == 1){
		return input_button;
	}
	else{
		return 17;
	}
}

void create_new_apple(){
	current_apple.x = sys_rand32_get() % 16;
	current_apple.y = sys_rand32_get() % 16;
}

void draw_snake_game(){
	uint16_t game_frame[16] = {0};

	for (int i = 0; i < snake_length; i++) {
		game_frame[snake_mask[i].y] |= 1 << snake_mask[i].x;
	}
	game_frame[current_apple.y] |= 1 << current_apple.x;

	ledmatrixSetMutexValue(game_frame);
}

void set_direction(uint8_t btnValue){
	switch (btnValue){
	case 0:
		if(snake_direction == up){
			snake_direction = left;
		}
		else{
			snake_direction--;
		}
		break;
	case 1:
		break;
	case 2:
		if(snake_direction == left){
			snake_direction = up;
		}
		else{
			snake_direction++;
		}
		break;
	default:
		break;
	}
}

//Return 1 upon a hit with the fruit
bool hit_detection_fruit(){
	if ((snake_mask[0].x == current_apple.x) && (snake_mask[0].y == current_apple.y)) {
        return 1;
    }
	else{
		return 0;
	}
}

//Return 1 upon a hit with the walls or other part of the snake
bool hit_detection_snake(){
	if((snake_mask[0].y > 15) || (snake_mask[0].y < 0)){
		return 1;
	}
	else if((snake_mask[0].x > 15) || (snake_mask[0].x < 0)){
		return 1;
	}
	for (int i = 0; i < snake_length; i++) {
		if((snake_mask[i].x == 17)){
			return 0;
		}
        for (int j = i + 1; j < snake_length; j++) {
            if ((snake_mask[i].x == snake_mask[j].x) && (snake_mask[i].y == snake_mask[j].y)) {
				return 1;
            }
        }
    }

	return 0;
}

//Updates the snake's position
void update_snake(){
	int i = 0;

	for(i = snake_length -1; i > 0; i--){
		snake_mask[i].x = snake_mask[i-1].x;
		snake_mask[i].y = snake_mask[i-1].y;
	}

	switch (snake_direction){
	case up:
		snake_mask[i].y = snake_mask[i].y - 1;
		break;
	case left:
		snake_mask[i].x = snake_mask[i].x + 1;
		break;
	case down:
		snake_mask[i].y = snake_mask[i].y + 1;
		break;
	case right:
		snake_mask[i].x = snake_mask[i].x - 1;
		break;
	default:
		break;
	}
}

static void wait_till_game_start(){
	lcdStringWrite("Druk op start");
	startledSet(1);

	while(startbuttonGet()){
		native_loop();
	}
	startledSet(0);
}

int playMg1() {
	uint8_t btnmatrix_off[4] = {0};
	uint8_t apples_caught = 0;
	uint8_t old_input = 17;
	uint8_t new_input = 17;
	uint16_t led_matrix_off[16] = {0};
	uint32_t score = 0;

	snake_mask[0].x = 7;
	snake_mask[0].y = 7;
	snake_length = 3;
	current_apple.x = 2;
	current_apple.y = 10;
	game_ongoing_mg1 = 1;

	show_oneliners(oneLinersMG1, MG1_ONELINERS);
	lcdEnable();
	wait_till_game_start();

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	lcdClear();
	k_timer_start(&mg1Timer, K_MSEC(mg1Timer_duration), K_NO_WAIT);

	while(game_ongoing_mg1){
		native_loop();
		new_input = get_abcbutton_input_number_untimed();
		if(new_input != old_input){
			old_input = new_input;
			set_direction(old_input);
		}
		// Handle snake related logic at a slower speed
		if(k_timer_remaining_get(&mg1Timer) == 0){
			update_snake();
			draw_snake_game();

			if(hit_detection_snake()){
				game_ongoing_mg1 = 0;
			}

			if(hit_detection_fruit()){
				create_new_apple();
				score = score + 100;
				apples_caught++;
				snake_length++;
			}

			if(apples_caught >= 10){
				game_ongoing_mg1 = 0;
			}

			k_timer_start(&mg1Timer, K_MSEC(mg1Timer_duration), K_NO_WAIT);
		}
	};

	printk("Score: %d\n", score);
	abcledsSet('a', 0);
	abcledsSet('c', 0);
	lcdDisable();
	btnmatrix_outSetMutexValue(btnmatrix_off);
	ledmatrixSetMutexValue(led_matrix_off);
	k_msleep(20);

	return (int)score;
}