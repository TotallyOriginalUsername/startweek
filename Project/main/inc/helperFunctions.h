#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include "threads.h"
#if defined(CONFIG_TEST_RNG_INPUT)
#include <zephyr/random/random.h>
#endif

#ifdef CONFIG_ARCH_POSIX
#define native_loop() k_sleep(K_MSEC(1))
#else
#define native_loop()
#endif

void btnmatrix_in_to_out(uint8_t* btnmatrix_input, uint8_t* btnmatrix_output);
void btnmatrix_to_ledmatrix(uint8_t* btnmatrix_shape, uint16_t* ledmatrix_shape);
void clear_btnmatrix_leds();
void show_correct();
void show_incorrect();
void show_oneliners(char input_array[][32], int lines);
void show_mg_score(int score);
void wait_till_abc_depressed();
void wait_till_btnmatrix_depressed();
void wait_till_switches_rest();
void get_abc_input(uint16_t input_time, uint8_t* input_array, size_t size);
void get_btnmatrix_input(uint16_t input_time, uint8_t* input_array, size_t size);
uint8_t get_btnmatrix_input_number(uint16_t input_time);
uint8_t get_btnmatrix_input_number_untimed();
void set_btnmatrix_led(uint8_t position);

void set_led_circle_dir_dist(int dir, int distance);
int get_relative_dir(int dir);
#endif //HELPERFUNCTIONS_H