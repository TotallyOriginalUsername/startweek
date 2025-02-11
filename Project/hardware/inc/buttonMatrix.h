#ifndef BUTTON_MATRIX_H
#define BUTTON_MATRIX_H

#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/kernel.h>

// GPIO definitions 
static const struct gpio_dt_spec buttonMatrixShiftDataIn 			= GPIO_DT_SPEC_GET(DT_ALIAS(shiftdatain4x4), gpios);
static const struct gpio_dt_spec buttonMatrixShiftOutputEnable 		= GPIO_DT_SPEC_GET(DT_ALIAS(shiftoutputenable4x4), gpios);
static const struct gpio_dt_spec buttonMatrixShiftClock 				= GPIO_DT_SPEC_GET(DT_ALIAS(shiftclock4x4 ), gpios);
static const struct gpio_dt_spec buttonMatrixMuxA 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxa4x4), gpios);
static const struct gpio_dt_spec buttonMatrixMuxB 					= GPIO_DT_SPEC_GET(DT_ALIAS(muxb4x4), gpios);

static const struct gpio_dt_spec buttonsButtonMatrix[16] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(button14x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button24x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button34x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button44x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button54x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button64x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button74x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button84x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button94x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button104x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button114x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button124x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button134x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button144x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button154x4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(button164x4), gpios)
};

#define BUTTONMATRIXLEDSINROW		4
#define BUTTONMATRIXROWS 			4

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

bool buttonMatrixConfig();
uint8_t buttonMatrixInit();
uint8_t buttonMatrixSet(uint8_t data[BUTTONMATRIXROWS]);

bool buttons4x4Config();
uint8_t buttons4x4Init();
uint8_t buttons4x4Get(uint8_t selectedbtn);

#endif // BUTTON_MATRIX_H