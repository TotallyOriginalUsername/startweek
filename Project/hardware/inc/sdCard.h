#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
#include <ff.h>
#endif
#include <string.h>
#include "shellFunctions.h"

int sd_card_init();
void sd_card_unmount();

int sd_setup_files();

int sd_clear_score();
int sd_add_score(int score);
int sd_get_score();
int sd_set_score(int score);

int sd_clear_progress();
int sd_get_progress();
int sd_set_progress(int progress);

uint8_t sd_get_buffer(uint16_t select_file, char *buf, size_t *len, size_t max_len);
uint8_t sd_get_trivia(uint8_t trivia_nr, char *buf, size_t *len, size_t max_len);

int sd_set_start_time(int16_t time);
int sd_set_end_time(int16_t time);
int16_t sd_get_start_time();
int16_t sd_get_end_time();

uint8_t sd_get_locations(struct location_new* locations, size_t size, int* locCount);
uint8_t sd_set_locations(struct location_new* locations, int locCount);

uint8_t sd_get_trivia_question(struct trivia_question* trivia);
uint8_t sd_save_trivia_question(struct trivia_question* trivia);

#endif // SDCARD_H