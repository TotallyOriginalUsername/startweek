#ifndef SDCARD_H
#define SDCARD_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
#include <ff.h>
#endif
#include <string.h>

int sd_card_init();
void sd_card_unmount();

int sd_clear_score();
int sd_get_score();
int sd_set_score(int score);

int sd_get_progress();
int sd_set_progress(int progress);

uint8_t sd_get_buffer(uint16_t select_file, char *buf, size_t *len, size_t max_len);

int16_t sd_get_start_time();
int16_t sd_get_end_time();

#endif // SDCARD_H