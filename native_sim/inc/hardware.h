#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/kernel.h>

#define LEDMATRIXLEDSINROW		16
#define LEDMATRIXROWS 			16
#define CIRCLEMATRIXLEDSINROW		8
#define CIRCLEMATRIXROWS 			8

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

static const struct gpio_dt_spec abcbutton[3] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(sw16), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw17), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw18), gpios),
};

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

static const struct gpio_dt_spec switchon[5] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(sw19), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw20), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw21), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw22), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw23), gpios),
};

static const struct gpio_dt_spec switchoff[5] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(sw24), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw25), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw26), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw27), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(sw28), gpios),
};

uint8_t abcbuttonsInit();
bool buttons4x4Config();
uint8_t buttons4x4Init();
uint8_t switchesInit();

uint8_t lcdStringWrite(char *msg);

uint8_t buttonsAbcGetLVGL(char selectedbtn);
uint8_t buttons4x4Get(uint8_t selectedbtn);
uint8_t buttons4x4GetLVGL(uint8_t selectedbtn);
uint8_t switchesGet(uint8_t selectedswitch);

uint8_t circleMatrixSet(uint8_t data[8]);
int8_t ledMatrixSet(int16_t data[16]);
uint8_t sevenSegmentSet(char input[4],uint8_t dpPosition);