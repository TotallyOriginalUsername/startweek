#include "minigame14.h"
#include "gyroCompass.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include <stdint.h>
LOG_MODULE_REGISTER(mg_14);
#define ball_timer_duration_mg14 100
#define score_timer_duration_mg14 10000
K_TIMER_DEFINE(ballTimer_mg14, NULL, NULL);
K_TIMER_DEFINE(scoreTimer_mg14, NULL, NULL);
#define level_count 4
#define minimum_score 100

struct point{
	uint8_t x;
	uint8_t y;
};

char *mg14Threads[mg14ThreadCount] = {"abcbtn", "ledmatrix"};

void getMg14Threads(char ***names, unsigned *amount) {
	*names = mg14Threads;
	*amount = mg14ThreadCount;
}

#define MG14_ONELINERS 3
char oneLinersMG14[MG14_ONELINERS][32] = {
	"Beweeg de bal   van linksboven",
	"naar rechtsonderdoor",
	"de doos te kantelen"
};

void get_input_x_mg14(struct point* player_ball, uint16_t walls[16]){
	int current_roll = 0;
	struct point new_position = *player_ball;
	gyroscope_get_roll(&current_roll);

	if((current_roll <= -7) && (player_ball->x < 15)){
		new_position.x++;
	}
	else if((current_roll >= 7) && (player_ball->x > 0)){
		new_position.x--;
	}
	if ((walls[new_position.y] & (1 << new_position.x)) == 0) {
		player_ball->x = new_position.x;
	}
}

void get_input_y_mg14(struct point* player_ball, uint16_t walls[16]){
	int current_pitch = 0;
	struct point new_position = *player_ball;
	gyroscope_get_pitch(&current_pitch);

	if((current_pitch <= -7) && (player_ball->y < 15)){
		new_position.y++;
	}
	else if((current_pitch >= 7) && (player_ball->y > 0)){
		new_position.y--;
	}
	if ((walls[new_position.y] & (1 << new_position.x)) == 0) {
		player_ball->y = new_position.y;
	}
}

void draw_game_mg14(struct point* player_ball, struct point* endgoal, uint16_t walls[16]){
	uint16_t game_frame[16] = {0};

	memcpy(game_frame, walls, sizeof(game_frame));

	game_frame[player_ball->y] = game_frame[player_ball->y] | (1 << player_ball->x);
	game_frame[endgoal->y] = game_frame[endgoal->y] | (1 << endgoal->x);

	ledmatrixSetMutexValue(game_frame);
}

bool hit_detection_mg14(struct point* player_ball, struct point* endgoal){
	if ((player_ball->x == endgoal->x) && (player_ball->y == endgoal->y)) {
        return 1;
    }
	else{
		return 0;
	}
}

int playMg14() {
	uint32_t score = 0;
	uint32_t level_score = minimum_score;
	uint32_t remaining_time = 0;
	uint16_t led_matrix_off[16] = {0};
	//char lcd_msg[32];
	uint8_t current_level = 0;
	struct point player_ball = {0,0};
	struct point endgoal = {15,15};
	uint16_t walls[level_count][16] = {{0b0000000000000000,0b0000000000010000,
		0b0000000000010000,0b0000001000010000,0b0000001000000000,
		0b0000001000000000,0b0000001000000000,0b0000001111111111,
		0b0000000000000000,0b0000000000000000,0b0000000000000000,
		0b1111000000000000,0b0000000000000000,0b0000000000000000,
		0b0000000000000000,0b0000000000000000},
	{0b0000010000001100,0b0000010000001100,0b0110010000001100,
		0b0110010000001100,0b0110010011001100,0b0010010011001100,
		0b0010010011001100,0b0010010011001100,0b0010010011001100,
		0b0010010011001100,0b0010010011000000,0b0010010011000000,
		0b0010010011000000,0b0010000011000000,0b0010000011000000,
		0b0010000011000000},
	{0b0000100000010000,0b0000100000010000,0b0000000011110000,
		0b0000000000010011,0b0011100000010000,0b0000100000010000,
		0b0000111100010000,0b1100100000011100,0b0000100000010000,
		0b0000100011110000,0b0011100000010000,0b0000100000010011,
		0b0000111100010000,0b1100100000000000,0b0000100001000000,
		0b0000100001000000},
	{0b0000000000000100,0b0010000000000100,0b0100011111100100,
		0b1000000000010100,0b0000001111001100,0b0010010000100100,
		0b0010100110010100,0b0010101001000100,0b0010101001100100,
		0b0011100110011100,0b0010010000000100,0b0010001111001000,
		0b0010100000010001,0b0010011111100010,0b0001000000000100,
		0b0000100000000000}};


	show_oneliners(oneLinersMG14, MG14_ONELINERS);
	lcdEnable();
	wait_till_game_start();
	k_timer_start(&ballTimer_mg14, K_MSEC(ball_timer_duration_mg14), K_NO_WAIT);
	k_timer_start(&scoreTimer_mg14, K_MSEC(score_timer_duration_mg14), K_NO_WAIT);

	while(current_level < level_count){
		native_loop();
		if(k_timer_remaining_get(&ballTimer_mg14) == 0){
			get_input_x_mg14(&player_ball, walls[current_level]);
			get_input_y_mg14(&player_ball, walls[current_level]);
			draw_game_mg14(&player_ball, &endgoal, walls[current_level]);
			k_timer_start(&ballTimer_mg14, K_MSEC(ball_timer_duration_mg14), K_NO_WAIT);
		}
		if(hit_detection_mg14(&player_ball, &endgoal)){
			remaining_time = k_timer_remaining_get(&scoreTimer_mg14);
			LOG_INF("remaining time: %d", remaining_time);
			player_ball.x = 0;
			player_ball.y = 0;
			level_score += remaining_time / 10;
			if(level_score >= 250){
				level_score = 250;
			}
			score += level_score;
			level_score = minimum_score;
			current_level++;
			k_timer_start(&scoreTimer_mg14, K_MSEC(score_timer_duration_mg14), K_NO_WAIT);
		}
	}

	LOG_INF("Score: %d\n", score);
	lcdClear();
	lcdDisable();
	ledmatrixSetMutexValue(led_matrix_off);
	k_msleep(50);
	
	return (int)score;
}
