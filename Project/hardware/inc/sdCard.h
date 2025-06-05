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

uint8_t sd_card_init();
void sd_card_unmount();

uint8_t sd_clear_score();
int sd_get_score();
uint8_t sd_set_score(int score);

uint8_t sd_get_locations(uint16_t type, char *buf, size_t *len, size_t max_len);

#endif // SDCARD_H