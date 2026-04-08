#include "minigame15.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
LOG_MODULE_REGISTER(mg_15);
void timer_handler_mg15(struct k_timer *timer_id);
#define tetris_piece_amount 7
#define max_timer_duration_mg15 550
#define min_timer_duration_mg15 150
#define input_timer_duration_mg15 75
K_TIMER_DEFINE(scrolldownTimer_mg15, NULL, NULL);
K_TIMER_DEFINE(inputTimer_mg15, timer_handler_mg15, NULL);
bool game_ongoing = true;
int next_pieces[3] = {};

typedef struct tetris_piece{
	uint8_t y;
	uint16_t piece[4];
} tetris_piece;

tetris_piece base_pieces[tetris_piece_amount] = {
	{0, {0b000001111000, 0b000000000000, 0b000000000000, 0b000000000000}},
	{0, {0b000000110000, 0b000000110000, 0b000000000000, 0b000000000000}},
	{0, {0b000000111000, 0b000000001000, 0b000000000000, 0b000000000000}},
	{0, {0b000000111000, 0b000000100000, 0b000000000000, 0b000000000000}},
	{0, {0b000000111000, 0b000000010000, 0b000000000000, 0b000000000000}},
	{0, {0b000000110000, 0b000000011000, 0b000000000000, 0b000000000000}},
	{0, {0b000000110000, 0b000001100000, 0b000000000000, 0b000000000000}}
};
tetris_piece current_piece;
uint16_t placed_pieces[16] = {0};

char *mg15Threads[mg15ThreadCount] = {"abcbtn", "ledmatrix"};

void getMg15Threads(char ***names, unsigned *amount) {
	*names = mg15Threads;
	*amount = mg15ThreadCount;
}

#define MG15_ONELINERS 3
char oneLinersMG15[MG15_ONELINERS][32] = {
	"Tetris! Gebruik de A(links) en",
	"C(rechts) knoppen om te bewegen",
	"en B(midden) om te roteren"
};

void draw_game_mg15(uint16_t UI[16]){
	uint16_t game_frame[16] = {0};
	int j = 0, k = 0;

	for (int i = 0; i < 16; i++) {
		game_frame[i] |= UI[i];
		game_frame[i] |= placed_pieces[i];
		if (i < 4) {
			game_frame[i] |= base_pieces[next_pieces[0]].piece[i] << 9;
			if (current_piece.y + i < 16) {
				game_frame[current_piece.y + i] |= current_piece.piece[i];
			}
		}
		if (i >= 6 && i < 10) {
			game_frame[i] |= (base_pieces[next_pieces[1]].piece[j++] << 9);
		}
		if (i >= 11 && i < 14) {
			game_frame[i] |= (base_pieces[next_pieces[2]].piece[k++] << 9);
		}
	}

	ledmatrixSetMutexValue(game_frame);
}

// true if current piece is on top of a placed piece, false otherwise
bool hit_detection_mg15(tetris_piece hitdetection_piece){
	for (int i = 0; i < 4; i++) {
		if ((hitdetection_piece.piece[i] & placed_pieces[i+hitdetection_piece.y]) != 0) {
			if(hitdetection_piece.y == 1){
				game_ongoing = false;
			}
			return true;
		}
		if (hitdetection_piece.piece[i] != 0 && hitdetection_piece.y + i >= 16) {
			return true;
		}
	}
	return false;
}

void rotate_piece(tetris_piece* new_piece){
	uint16_t second_temp[4] = {0};
	uint16_t row_mask = 0b0000010000000000;
	int size = 4;
	int x = 12;
	int new_x = 12;
	int offset = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 11; j++) {
			if (new_piece->piece[i] & (1 << j)) {
				if (j < x) {
					x = j;
				}
			}
		}
	}

	for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
			if ((new_piece->piece[size - 1 - j] >> (x + i)) & 1) {
				second_temp[i] |= (1 << (j + 2));
			}
        }
    }
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 11; j++) {
			if (second_temp[i] & (1 << j)) {
				if (j < new_x) {
					new_x = j;
				}
			}
		}
	}
	offset = new_x - x;

	for(int i = 0; i < 4; i++){
		if (offset < 0) {
			second_temp[i] = second_temp[i] << -offset;
		}
		else if (offset > 0) {
			second_temp[i] = second_temp[i] >> offset;
		}
		if (second_temp[i] & row_mask) {
			return;
		}
	}

	for(int i = 0; i < 4; i++){
		new_piece->piece[i] = second_temp[i];
	}
}

// Scrolls down the current piece
void scroll_down_mg15(){
	tetris_piece new_piece = current_piece;
	new_piece.y++;

	if (hit_detection_mg15(new_piece)) {
		for (int i = 0; i < 4; i++){
			if (current_piece.y + i < 16) {
				placed_pieces[current_piece.y + i] |= current_piece.piece[i];
			}
		}
		// queue-like behaviour
		current_piece = base_pieces[next_pieces[0]];
		for (int j = 0; j < 2; j++) {
			next_pieces[j] = next_pieces[j+1];
		}
		next_pieces[2] = sys_rand32_get() % tetris_piece_amount;
	} else {
		current_piece.y++;
	}
}

uint8_t check_rows_cleared(){
	uint16_t row_mask = 0b0000001111111111;
	uint8_t row_count = 0;
	for (int i = 15; i > 0; i--) {
		if ((placed_pieces[i] & row_mask) == row_mask) {
			for (int j = i; j > 0; j--) {
				placed_pieces[j] = placed_pieces[j - 1];
			}
			row_count++;
		}
	}
	return row_count;
}

void timer_handler_mg15(struct k_timer *timer_id){
	uint8_t *abcbtns;
	uint8_t *button = (uint8_t *)k_timer_user_data_get(&inputTimer_mg15);
	tetris_piece new_piece = current_piece;

	abcbtns = abcbtnGetMutexValue();

	if(abcbtns[*button] == 0 && *button == 0){
		for (int i = 0; i < 4; i++) {
			if (new_piece.piece[i] & (1 << 9)) {
				return;
			}
			new_piece.piece[i] <<= 1;
		}
	}
	else if(abcbtns[*button] == 0 && *button == 2){
		for (int i = 0; i < 4; i++) {
			if (new_piece.piece[i] & (1 << 0)) {
				return;
			}
			new_piece.piece[i] >>= 1;
		}
	}
	if(!hit_detection_mg15(new_piece)){
		current_piece = new_piece;
	}
}

void get_input_mg15(){
	uint8_t *abcbtns;
	static uint8_t button = 0;
	abcbtns = abcbtnGetMutexValue();
	tetris_piece new_piece = current_piece;

	if((abcbtns[0] == 0) && (abcbtns[2] == 0)){
		return;
	}

	if(abcbtns[0] == 0){
		if(k_timer_remaining_get(&inputTimer_mg15) == 0){
		button = 0;
		k_sleep(K_MSEC(1));
		k_timer_user_data_set(&inputTimer_mg15, &button);
		k_timer_start(&inputTimer_mg15, K_MSEC(input_timer_duration_mg15), K_NO_WAIT);
		}
		else{
			//timer already running
		}
	}
	else if(abcbtns[2] == 0){
		if(k_timer_remaining_get(&inputTimer_mg15) == 0){
		button = 2;
		k_sleep(K_MSEC(1));
		k_timer_user_data_set(&inputTimer_mg15, &button);
		k_timer_start(&inputTimer_mg15, K_MSEC(input_timer_duration_mg15), K_NO_WAIT);
		}
	}
	else if (abcbtns[1] == 0) {
		if(button != 1){
			button = 1;
			rotate_piece(&new_piece);
			if(!hit_detection_mg15(new_piece)){
				current_piece = new_piece;
			}
		}
	}
	else{
		button = 3;
	}
}

int playMg15() {
	uint32_t score = 0;
	int16_t dynamic_time = 0;
	uint16_t led_matrix_off[16] = {0};
	uint16_t UI[16] = {
		0b0000010000000000,0b0000010000000000,0b0000010000000000,0b0000010000000000,
		0b0000010000000000,0b0000010000000000,0b0000010000000000,0b0000010000000000,
		0b0000010000000000,0b0000010000000000,0b0000010000000000,0b0000010000000000,
		0b0000010000000000,0b0000010000000000,0b0000010000000000,0b0000010000000000};
	uint8_t rows_cleared = 0;

	current_piece = base_pieces[0];
	game_ongoing = true;
	for (int i = 0; i < 3; i++) {
		next_pieces[i] = sys_rand32_get() % tetris_piece_amount;
	}
	for (int i = 0; i < 16; i++) {
		placed_pieces[i] = 0;
	}

	show_oneliners(oneLinersMG15, MG15_ONELINERS);
	lcdEnable();
	wait_till_game_start();
	abcledsSet('a', 1);
	abcledsSet('b', 1);
	abcledsSet('c', 1);
	k_timer_start(&scrolldownTimer_mg15, K_MSEC(min_timer_duration_mg15), K_NO_WAIT);

	while(game_ongoing){
		native_loop();
		get_input_mg15();
		if(k_timer_remaining_get(&scrolldownTimer_mg15) == 0){
			scroll_down_mg15();
			rows_cleared = check_rows_cleared();
			score += rows_cleared * 100;
			dynamic_time = max_timer_duration_mg15 - (score / 2);
			if (dynamic_time <= min_timer_duration_mg15) {
				dynamic_time = min_timer_duration_mg15;
			}
			k_timer_start(&scrolldownTimer_mg15, K_MSEC(dynamic_time), K_NO_WAIT);
		}
		draw_game_mg15(UI);
	}

	if (score > 1000) {
		score = 1000;
	}

	LOG_INF("Score: %d\n", score);
	lcdClear();
	lcdDisable();
	ledmatrixSetMutexValue(led_matrix_off);
	abcledsSet('a', 0);
	abcledsSet('b', 0);
	abcledsSet('c', 0);
	k_msleep(50);
	
	return (int)score;
}
