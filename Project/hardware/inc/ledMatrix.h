#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/kernel.h>

// GPIO definitions 
static const struct gpio_dt_spec ledMatrixShiftDataIn 			= GPIO_DT_SPEC_GET(DT_ALIAS(shiftdatain16x16), gpios);
static const struct gpio_dt_spec ledMatrixShiftOutputEnable 	= GPIO_DT_SPEC_GET(DT_ALIAS(shiftenable16x16), gpios);
static const struct gpio_dt_spec ledMatrixShiftClock 			= GPIO_DT_SPEC_GET(DT_ALIAS(shiftclock16x16 ), gpios);
static const struct gpio_dt_spec ledMatrixMuxA 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxa16x16), gpios);
static const struct gpio_dt_spec ledMatrixMuxB 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxb16x16), gpios);
static const struct gpio_dt_spec ledMatrixMuxC 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxc16x16), gpios);
static const struct gpio_dt_spec ledMatrixMuxD 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxd16x16), gpios);

#define LEDMATRIXLEDSINROW		16
#define LEDMATRIXROWS 			16

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

bool ledMatrixConfig();
int8_t ledMatrixInit();
int8_t ledMatrixSet(int16_t data[16]);

#endif // LED_MATRIX_H