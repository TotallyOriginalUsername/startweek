#include "minigame1.h"

K_TIMER_DEFINE(mg1Timer, NULL, NULL);

LOG_MODULE_REGISTER(mg_1);

#define mg1Timer_duration 200
#define snake_max_length 15

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

void create_new_apple(struct apple_location* current_apple){
	current_apple->x = sys_rand32_get() % 16;
	current_apple->y = sys_rand32_get() % 16;
}

void draw_snake_game(struct apple_location* current_apple,
	struct snake_locations* snake_mask, uint8_t snake_length){
	uint16_t game_frame[16] = {0};

	for (int i = 0; i < snake_length; i++) {
		game_frame[snake_mask[i].y] |= 1 << snake_mask[i].x;
	}
	game_frame[current_apple->y] |= 1 << current_apple->x;

	ledmatrixSetMutexValue(game_frame);
}

void set_snake_direction(uint8_t btnValue, uint8_t* snake_direction){
	switch (btnValue){
	case 0:
		if(*snake_direction == up){
			*snake_direction = left;
		}
		else{
			(*snake_direction)--;
		}
		break;
	case 1:
		break;
	case 2:
		if(*snake_direction == left){
			*snake_direction = up;
		}
		else{
			(*snake_direction)++;
		}
		break;
	default:
		break;
	}
}

//Return 1 upon a hit with the fruit
bool hit_detection_fruit(struct apple_location* current_apple, struct snake_locations* snake_mask){
	if ((snake_mask[0].x == current_apple->x) && (snake_mask[0].y == current_apple->y)) {
        return 1;
    }
	else{
		return 0;
	}
}

//Return 1 upon a hit with the walls or other part of the snake
bool hit_detection_snake(uint8_t snake_length, struct snake_locations* snake_mask){
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
void update_snake(uint8_t snake_direction, struct snake_locations* snake_mask, uint8_t snake_length){
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

int playMg1() {
	bool game_ongoing_mg1 = 1;
	uint8_t btnmatrix_off[4] = {0};
	uint8_t apples_caught = 0;
	uint8_t old_input = 17;
	uint8_t new_input = 17;
	uint8_t snake_direction = up;
	uint8_t snake_length = 3;
	uint16_t led_matrix_off[16] = {0};
	uint32_t score = 0;

	struct apple_location current_apple = {2, 10};
	struct snake_locations snake_mask[snake_max_length] = {[0 ... snake_max_length - 1] = {17, 17}};

	snake_mask[0].x = 7;
	snake_mask[0].y = 7;
	snake_length = 3;

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
			set_snake_direction(old_input, &snake_direction);
		}
		// Handle snake related logic at a slower speed
		if(k_timer_remaining_get(&mg1Timer) == 0){
			update_snake(snake_direction, snake_mask, snake_length);
			draw_snake_game(&current_apple, snake_mask, snake_length);

			if(hit_detection_snake(snake_length, snake_mask)){
				game_ongoing_mg1 = 0;
			}

			if(hit_detection_fruit(&current_apple, snake_mask)){
				create_new_apple(&current_apple);
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

	LOG_INF("Score: %d", score);
	abcledsSet('a', 0);
	abcledsSet('c', 0);
	lcdDisable();
	btnmatrix_outSetMutexValue(btnmatrix_off);
	ledmatrixSetMutexValue(led_matrix_off);
	k_msleep(20);

	return (int)score;
}