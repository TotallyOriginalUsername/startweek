/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>
#include "statemachine.h"
#include "threads.h"
#include "lvgl_ui.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

// The stack size of all threads
#define STACKSIZE 1024

//LVGL stack
#define STACKSIZE2 16000

// Thread priority values (lower value is higher priority)
#define TMAIN_PRIORITY 9
#define TLVGL_PRIORITY 3

void lvgl_task_handler_loop() {
    while (1) {
		//k_cpu_idle();
        lv_task_handler();
		//for native_sim
        k_sleep(K_MSEC(1));
    }
}

int tmain() // Core thread
{
	const struct device *display_dev;
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	//lv_task_handler();
	display_blanking_off(display_dev);

	setup_ui(lv_scr_act());

	printf("Main\n");
	startStatemachine();

	return 0;
}

// Define the threads
K_THREAD_DEFINE(tmain_id, STACKSIZE, tmain, NULL, NULL, NULL, TMAIN_PRIORITY, 0, 0);

K_THREAD_DEFINE(tlvgl_id, STACKSIZE2, lvgl_task_handler_loop, NULL, NULL, NULL, TLVGL_PRIORITY, 0, 0);