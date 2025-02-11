#include "lcd.h"

//is located in c file as private variable so that it is included ones and only used by lcd.c
bool lastaction;


/** 
 * @brief Writes 8 bit value to lcd.
 * 
 * writes 8 bit data or instruction value to lcd.
 * 
 * @return Does not return
 */ 
void lcdWrite8bits(uint8_t bits)
{
	gpio_pin_set_dt(&lcdE,LOW);
	k_sleep(K_USEC(1));

	gpio_pin_set_dt(&lcdData0,LOW);
	gpio_pin_set_dt(&lcdData1,LOW);
	gpio_pin_set_dt(&lcdData2,LOW);
	gpio_pin_set_dt(&lcdData3,LOW);
	gpio_pin_set_dt(&lcdData4,LOW);
	gpio_pin_set_dt(&lcdData5,LOW);
	gpio_pin_set_dt(&lcdData6,LOW);
	gpio_pin_set_dt(&lcdData7,LOW);

	if ((bits & BIT(0)) == BIT(0)) {
		gpio_pin_set_dt(&lcdData0,HIGH);
	}
	if ((bits & BIT(1)) == BIT(1)) {
		gpio_pin_set_dt(&lcdData1,HIGH);
	}
	if ((bits & BIT(2)) == BIT(2)) {
		gpio_pin_set_dt(&lcdData2,HIGH);
	}
	if ((bits & BIT(3)) == BIT(3)) {
		gpio_pin_set_dt(&lcdData3,HIGH);
	}
	if ((bits & BIT(4)) == BIT(4)) {
		gpio_pin_set_dt(&lcdData4,HIGH);
	}
	if ((bits & BIT(5)) == BIT(5)) {
		gpio_pin_set_dt(&lcdData5,HIGH);
	}
	if ((bits & BIT(6)) == BIT(6)) {
		gpio_pin_set_dt(&lcdData6,HIGH);
	}
	if ((bits & BIT(7)) == BIT(7)) {
		gpio_pin_set_dt(&lcdData7,HIGH);
	}

	gpio_pin_set_dt(&lcdE,HIGH);
	k_sleep(K_USEC(1));
	gpio_pin_set_dt(&lcdE,LOW);
	k_sleep(K_USEC(1));

}


/** 
 * @brief Writes instructions to the lcd
 * 
 * Pin for instruction is set and instruction is send.
 * 
 * @return Does not return
 */ 
void lcdWriteInstruction(uint8_t bits)
{
	if(lastaction == actiondata)
	{
		gpio_pin_set_dt(&lcdRS,LOW);
		k_sleep(K_MSEC(1));
	}
	lastaction = actioninstruction;
	lcdWrite8bits(bits);
}

/** 
 * @brief Writes data to the lcd
 * 
 * Pin for data is set and data is send.
 * 
 * @return Does not return
 */ 
void lcdWriteData(uint8_t bits)
{
	if(lastaction == actioninstruction)
	{
		gpio_pin_set_dt(&lcdRS,HIGH);
		k_sleep(K_MSEC(1));
	}
	lastaction = actiondata;
	lcdWrite8bits(bits);
}


/** 
 * @brief Configures the lcd.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool lcdConfig()
{
	//Checks if gpio is available
	if (!gpio_is_ready_dt(&lcdData1) && !gpio_is_ready_dt(&lcdData2) &&
		!gpio_is_ready_dt(&lcdData3) && !gpio_is_ready_dt(&lcdData4) &&
		!gpio_is_ready_dt(&lcdData5) && !gpio_is_ready_dt(&lcdData6) &&
		!gpio_is_ready_dt(&lcdData7) && !gpio_is_ready_dt(&lcdData0) &&
		!gpio_is_ready_dt(&lcdE) && !gpio_is_ready_dt(&lcdRS) && !gpio_is_ready_dt(&lcdBacklightEnable)) 
	{
		return 1;
	}
	//configures the gpio
	int ret = 0;
	ret += gpio_pin_configure_dt(&lcdData0, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData1, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData2, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData3, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData4, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData5, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData6, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdData7, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdE, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdRS, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&lcdBacklightEnable, GPIO_OUTPUT_ACTIVE);
	//return when gpio is configured incorrectly
	if (ret != 0) 
	{
		return 1;
	}

	return 0;
}

/** 
 * @brief Initializes the lcd
 * 
 * Sets all pins from floating to 0
 * Configures the lcd with predefined settings.
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
int8_t lcdInit()
{
	int ret = 0;
	ret += gpio_pin_set_dt(&lcdData0,LOW);
	ret += gpio_pin_set_dt(&lcdData1,LOW);
	ret += gpio_pin_set_dt(&lcdData2,LOW);
	ret += gpio_pin_set_dt(&lcdData3,LOW);
	ret += gpio_pin_set_dt(&lcdData4,LOW);
	ret += gpio_pin_set_dt(&lcdData5,LOW);
	ret += gpio_pin_set_dt(&lcdData6,LOW);
	ret += gpio_pin_set_dt(&lcdData7,LOW);
	ret += gpio_pin_set_dt(&lcdE,LOW);
	ret += gpio_pin_set_dt(&lcdRS,LOW);
	ret += gpio_pin_set_dt(&lcdBacklightEnable,LOW);
	if(ret != 0)
	{
		return 1;
	}

	lastaction = actiondata;
	k_sleep(K_MSEC(50)); // Wait for startup
	
	// Reset LCD
	lcdWriteInstruction(FunctionReset);
	k_sleep(K_MSEC(10));			/* wait for 10ms */
	
	lcdWriteInstruction(FunctionReset);
	k_sleep(K_USEC(200));			/* wait for 200us */

	lcdWriteInstruction(FunctionReset);
	k_sleep(K_USEC(200));			/* wait for 200us */
	// end Reset lcd

	lcdWriteInstruction(FunctionSet8bit);
	k_sleep(K_USEC(80));			/* wait for 80us */

	lcdWriteInstruction(DisplayOff);
	k_sleep(K_USEC(80));			/* wait for 80us */
	
	lcdWriteInstruction(Clear);
	k_sleep(K_USEC(80));			/* wait for 80us */

	lcdWriteInstruction(EntryMode);
	k_sleep(K_USEC(80));			/* wait for 80us */

	lcdWriteInstruction(DisplayOn);
	k_sleep(K_USEC(80));			/* wait for 80us */

	return 0;
}
/** 
 * @brief Writes characters to the screen.
 * 
 * Clears the screen and returns to home.
 * Writes cahraters and number to the screen.
 * 
 * @return Returns a 0 on succes and a 1 on error.
 * error can be a size of 0 or a size of > 32
 */ 
uint8_t lcdStringWrite(char *msg)
{
	int i;
	int len = 0;
	uint8_t data;
	
	lcdWriteInstruction(Clear);
	lcdWriteInstruction(Home);

	len = strlen(msg);
	if (len != 0 && len <= 16)
	{
		for (i = 0; i < len; i++) {
		data = msg[i];
		lcdWriteData(data);
		}
	}
	else if (len != 0 && len <= 32) {
		for (i = 0; i < 16; i++) {
		data = msg[i];
		lcdWriteData(data);
		}	
	
		lcdWriteInstruction(SetCursor | LineTwo);
		k_sleep(K_USEC(80));

		for (i = 16; i < len; i++) {
		data = msg[i];
		lcdWriteData(data);
		}	
	}
	else {
		return 1;
	}
	return 0;	
}

/** 
 * @brief Enables the backlight.
 * 
 * Enables the backlight by setting the pin to high
 * 
 * @return Always return a 0
 */ 
uint8_t lcdEnable()
{
	gpio_pin_set_dt(&lcdBacklightEnable,HIGH);
	return 0;
}

/** 
 * @brief Disables the backlight.
 * 
 * Disables the backlight by setting the pin to low
 * 
 * @return Always return a 0
 */ 
uint8_t lcdDisable()
{
	gpio_pin_set_dt(&lcdBacklightEnable,LOW);
	return 0;
}

/** 
 * @brief Clears characters
 * 
 * Clears character from screen and returns to home.
 * 
 * @return Always return a 0
 */ 
uint8_t lcdClear()
{
	lcdWriteInstruction(Clear);
	lcdWriteInstruction(Home);
	return 0;
}