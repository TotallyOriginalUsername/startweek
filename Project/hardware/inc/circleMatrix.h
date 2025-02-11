#ifndef CIRCLE_MATRIX_H
#define CIRCLE_MATRIX_H

#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/kernel.h>

// GPIO definitions 
static const struct gpio_dt_spec circleMatrixShiftDataIn 			= GPIO_DT_SPEC_GET(DT_ALIAS(shiftdataincircle), gpios);
static const struct gpio_dt_spec circleMatrixShiftOutputEnable 		= GPIO_DT_SPEC_GET(DT_ALIAS(shiftoutputenablecircle), gpios);
static const struct gpio_dt_spec circleMatrixShiftClock 			= GPIO_DT_SPEC_GET(DT_ALIAS(shiftclockcircle ), gpios);
static const struct gpio_dt_spec circleMatrixMuxA 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxacircle), gpios);
static const struct gpio_dt_spec circleMatrixMuxB 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxbcircle), gpios);
static const struct gpio_dt_spec circleMatrixMuxC 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxccircle), gpios);

#define CIRCLEMATRIXLEDSINROW		8
#define CIRCLEMATRIXROWS 			8

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

bool circleMatrixConfig();
uint8_t circleMatrixInit();
uint8_t circleMatrixSet(uint8_t data[8]);

#endif // CIRCLE_MATRIX_H