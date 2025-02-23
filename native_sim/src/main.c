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
	char abcButton = 'a';
	char char_input[4] = {'4', '3', '2', '0'};
	char char_input2[4] = {'5', '6', '7', '8'};
	char msg[6] = "yellow";
	uint8_t dpPosition = 0;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	int16_t bad_apple_frame[16] = {
        0b0000000001000000,
        0b0000000001110000,
        0b0000000011111000,
        0b0000000111111100,
        0b0000000111111100,
        0b0000000111111100,
        0b0000000111111100,
        0b0000000011111110,
        0b0000000001111110,
        0b0000100001111110,
        0b0001100001111000,
        0b0001110001111000,
        0b0000111011111000,
        0b0000111111111000,
        0b0000111111111110,
        0b0000111101111100
    };

	int8_t led_circle_data[8] = 	{0b10101010, 0b11111111, 0b10101010, 0b10101010, 
						0b10101010, 0b10101010, 0b10101010, 0b10101010,
						};

	//might not be needed, abc buttons worked without
	abcbuttonsInit();
	buttons4x4Config();
	buttons4x4Init();
	switchesInit();

	lv_task_handler();
	display_blanking_off(display_dev);

	setup_ui(lv_scr_act());

	circleMatrixSet(led_circle_data);
	ledMatrixSet(bad_apple_frame);
	sevenSegmentSet(char_input, dpPosition);

	lcdStringWrite(msg);
	sevenSegmentSet(char_input2, dpPosition);

	while (1) {
		lv_task_handler();
		/*
		for(int i = 0; i < 16; i++){
			uint8_t button_state = buttons4x4GetLVGL(i);
			if (button_state < 0) {
				printk("Failed to read button state: %d", button_state);
			} else {
				printk("Button %d state: %s\n", i, button_state ? "Pressed" : "Released");
			}
		}
		printk("\n ----------- \n");
		*/
		for(uint8_t j = 0; j < 5; j++)
		{
			uint8_t switchState = switchesGet(j);
			printk("Switch state: %d\n", switchState);
		}
		
	for(int i = 0; i < 10; i++){
		sprintf(msg, "%d", i);
		sprintf(char_input2, "%d", i);
		lcdStringWrite(msg);
		sevenSegmentSet(char_input2, dpPosition);
		printf("msg: %s\n", msg);
		printf("%d\n", i);
	}

		k_sleep(K_MSEC(10));
	}
}