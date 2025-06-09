
#include "statemachine.h"
#include "threads.h"

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

// The stack size of all threads
#define STACKSIZE 1024

// Give the main thread enough stack size to handle the sd card
#define STACKSIZE_MAIN 8192 * 2

// Thread priority values (lower value is higher priority)
#define TMAIN_PRIORITY 8

#if defined(CONFIG_ARCH_POSIX)
#define STACKSIZE2 16000
#define TLVGL_PRIORITY 3

void lvgl_task_handler_loop() {
    while (1) {
		//k_cpu_idle();
        lv_task_handler();
		//for native_sim
        k_sleep(K_MSEC(1));
    }
}
#endif

void tmain(void) // Core thread
{
	#if defined(CONFIG_ARCH_POSIX)
	const struct device *display_dev;
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
	}

	display_blanking_off(display_dev);

	setup_ui(lv_scr_act());
	#endif
	LOG_INF("Main\n");

	startStatemachine();
}

// Define the threads
K_THREAD_DEFINE(tmain_id, STACKSIZE_MAIN, tmain, NULL, NULL, NULL, TMAIN_PRIORITY, 0, 0);
#if defined(CONFIG_ARCH_POSIX)
K_THREAD_DEFINE(tlvgl_id, STACKSIZE2, lvgl_task_handler_loop, NULL, NULL, NULL, TLVGL_PRIORITY, 0, 0);
#endif