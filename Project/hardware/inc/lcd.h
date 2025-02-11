#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <string.h>

// GPIO definitions 
static const struct gpio_dt_spec lcdRS 					= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayrs), gpios);
static const struct gpio_dt_spec lcdE 					= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplaye), gpios);
static const struct gpio_dt_spec lcdData0 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd0), gpios);
static const struct gpio_dt_spec lcdData1 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd1), gpios);
static const struct gpio_dt_spec lcdData2 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd2), gpios);
static const struct gpio_dt_spec lcdData3 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd3), gpios);
static const struct gpio_dt_spec lcdData4 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd4), gpios);
static const struct gpio_dt_spec lcdData5 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd5), gpios);
static const struct gpio_dt_spec lcdData6 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd6), gpios);
static const struct gpio_dt_spec lcdData7 				= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplayd7), gpios);
static const struct gpio_dt_spec lcdBacklightEnable		= GPIO_DT_SPEC_GET(DT_ALIAS(lcddisplaybacklightenable), gpios);

// LCD instructions
#define Clear           0b00000001          // replace all characters with ASCII 'space'
#define Home            0b00000010          // return cursor to first position on first line
#define EntryMode       0b00000110          // shift cursor from left to right on read/write
#define DisplayOff      0b00001000          // turn display off
#define DisplayOn       0b00001100          // display on, cursor off, don't blink character
#define FunctionReset   0b00110000          // reset the LCD
#define FunctionSet8bit 0b00111000          // 8-bit data, 2-line display, 5 x 7 font
#define SetCursor       0b10000000          // set cursor position
#define LineOne     0x00                    // start of line 1
#define LineTwo     0x40                    // start of line 2

// local variable
#define actioninstruction 0
#define actiondata 1

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

bool lcdConfig();
int8_t lcdInit();
uint8_t lcdStringWrite(char *msg);
uint8_t lcdEnable();
uint8_t lcdDisable();
uint8_t lcdClear();

#endif // LCD_H