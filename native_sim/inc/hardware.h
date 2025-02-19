#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/kernel.h>

#define LEDMATRIXLEDSINROW		16
#define LEDMATRIXROWS 			16
#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

static const struct gpio_dt_spec buttonsButtonMatrix[16] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw4), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw5), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw6), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw7), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw8), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw9), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw10), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw11), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw12), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw13), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw14), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw15), gpios)
};

bool buttons4x4Config();
uint8_t buttons4x4Init();
uint8_t buttons4x4Get(uint8_t selectedbtn);
uint8_t buttons4x4GetLVGL(uint8_t selectedbtn);

int8_t ledMatrixSet(int16_t data[16]);