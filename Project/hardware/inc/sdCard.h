#ifndef SDCARD_H
#define SDCARD_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>
#include <zephyr/sys/printk.h>
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
#include <ff.h>
#endif
#include <string.h>

uint8_t sd_card_init();
void sd_card_unmount();

uint8_t sd_clear_score();
int sd_get_score();
uint8_t sd_set_score(int score);

#endif // SDCARD_H