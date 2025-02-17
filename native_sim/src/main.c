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
#include "../inc/lvgl_ui.h"
#include "../inc/hardware.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static void lv_btn_click_callback(lv_event_t *e)
{
	//touch input
	ARG_UNUSED(e);
	printk("Button pressed\n");
}

int main(void)
{
	const struct device *display_dev;
	lv_obj_t *hello_world_label;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	buttons4x4Config();
	buttons4x4Init();

	//lv_obj_t * cont_screen = lv_obj_create(lv_scr_act());
	//lv_obj_set_size(cont_screen, 490, 490);
    //lv_obj_align(cont_screen, LV_ALIGN_CENTER, 0, 0);
	//task_handler needs to be called after setting cont size
	lv_task_handler();
	display_blanking_off(display_dev);

	setup_ui(lv_scr_act());

	while (1) {
		lv_task_handler();
		for(int i = 0; i < 1; i++){
			uint8_t button_state = buttons4x4GetLVGL(i);
			if (button_state < 0) {
				printk("Failed to read button state: %d", button_state);
			} else {
				printk("Button %d state: %s\n", i, button_state ? "Pressed" : "Released");
			}
		}
		printk("\n ----------- \n");

		k_sleep(K_MSEC(10));
	}
}