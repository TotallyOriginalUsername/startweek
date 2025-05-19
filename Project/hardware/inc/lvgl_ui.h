#ifndef LVGL_UI_H
#define LVGL_UI_H
#if defined(CONFIG_ARCH_POSIX)
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>
#include <math.h>

void set_abc_button(int button, bool value);
void set_start_button(bool value);
void set_button(int button, int state);
void set_lcd_display(char *msg);
void set_led(int led, int state);
void set_led_circle(int led, int state);
void set_segmented_display(char input[4],uint8_t dpPosition);

uint8_t get_button_state(uint8_t selectedbtn);
uint8_t get_button_abc_state(char selectedbtn);
uint8_t get_button_start();
uint8_t get_switch(uint8_t selectedswitch);
/* Setup UI*/
void setup_ui(lv_obj_t * parent);
#endif

#endif // LVGL_UI_H