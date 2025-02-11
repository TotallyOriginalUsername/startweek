#include "ledMatrix.h"

/** 
 * @brief Shifts one bit of data in the shift register. 
 * 
 * Shifts one bit of data in the shift register by setting the correct value on the shiftdatain16x16 pin.
 * After setting the value the Clock is toggled so the bit is set in the register.
 * 
 * @param[in] ShiftDataValue – Value of pin when shifting data 
 * 
 * @return No return
 */ 
void ledMatrixSendOneBitData(bool ShiftDataValue)
{
	gpio_pin_set_dt(&ledMatrixShiftDataIn,ShiftDataValue);
	gpio_pin_set_dt(&ledMatrixShiftClock,HIGH);
	gpio_pin_set_dt(&ledMatrixShiftClock,LOW);
}

/** 
 * @brief Configures the ledMatrix.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool ledMatrixConfig()
{
	//Checks if gpio is available
	if (!gpio_is_ready_dt(&ledMatrixShiftDataIn) && !gpio_is_ready_dt(&ledMatrixShiftOutputEnable) &&
		!gpio_is_ready_dt(&ledMatrixShiftClock) && !gpio_is_ready_dt(&ledMatrixMuxA) &&
		!gpio_is_ready_dt(&ledMatrixMuxB) && !gpio_is_ready_dt(&ledMatrixMuxC) && !gpio_is_ready_dt(&ledMatrixMuxD)) 
	{
		return 1;
	}
	//configures the gpio
	uint8_t ret = 0;
	ret += gpio_pin_configure_dt(&ledMatrixShiftDataIn, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&ledMatrixShiftOutputEnable, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&ledMatrixShiftClock, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&ledMatrixMuxA, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&ledMatrixMuxB, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&ledMatrixMuxC, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&ledMatrixMuxD, GPIO_OUTPUT_ACTIVE);
	//return when gpio is configured incorrectly
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief Initializes the ledMatrix
 * 
 * Sets all pins from floating to 0
 * Clears the Shift registers by writing 16 times a 0.
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
int8_t ledMatrixInit ()
{
	uint8_t ret = 0;
	ret += gpio_pin_set_dt(&ledMatrixShiftDataIn,LOW);
	ret += gpio_pin_set_dt(&ledMatrixShiftOutputEnable,LOW);
	ret += gpio_pin_set_dt(&ledMatrixShiftClock,LOW);
	ret += gpio_pin_set_dt(&ledMatrixMuxA,LOW);
	ret += gpio_pin_set_dt(&ledMatrixMuxB,LOW);
	ret += gpio_pin_set_dt(&ledMatrixMuxC,LOW);
	ret += gpio_pin_set_dt(&ledMatrixMuxD,LOW);

	if (ret != 0) 
	{
		return 1;
	}
	for (size_t row = 0; row < LEDMATRIXROWS; row++)
	{
		for (int i = 0; i < LEDMATRIXLEDSINROW; i++)
		{
			ledMatrixSendOneBitData(LOW);
		}
		gpio_pin_set_dt(&ledMatrixShiftOutputEnable,HIGH);
		gpio_pin_set_dt(&ledMatrixShiftOutputEnable,LOW);

		gpio_pin_set_dt(&ledMatrixMuxA,(row & 0x1));
		gpio_pin_set_dt(&ledMatrixMuxB,(row & 0x2));
		gpio_pin_set_dt(&ledMatrixMuxC,(row & 0x4));
		gpio_pin_set_dt(&ledMatrixMuxD,(row & 0x8));
	}

	return 0;
}

/** 
 * @brief Writes data to the whole matrix
 * 
 * By reading the values from the data variable each bit is defined and will be turned on in te correct sequence.
 * Each row is shifted into a registers and after each row the correct Mux is enabled to select the correct row.
 * 
 * @param[in] data – Array of int16_t with the size of ROWS. Every bit is equal to one led in the matrix.
 * 
 * @return Returns 0 if Configuration and initialization have been performs.
 * Returns a 1 if the Configuration has not been perfomed.
 * Returns a 2 if the Initialiazation has not been perfomed.
 * Returns a 3 if the Configuration and the Initialiazation has not been perfomed.
 */ 
int8_t ledMatrixSet(int16_t data[LEDMATRIXROWS])
{
	for (size_t row = 0; row < LEDMATRIXROWS; row++)
	{
		for (size_t led = 0; led < LEDMATRIXLEDSINROW; led++)
		{
			if(data[row] & 0x1<<led)
			{
				ledMatrixSendOneBitData(HIGH);
			}
			else
			{
				ledMatrixSendOneBitData(LOW);
			}
		}

		gpio_pin_set_dt(&ledMatrixShiftOutputEnable,HIGH);
		gpio_pin_set_dt(&ledMatrixShiftOutputEnable,LOW);

		gpio_pin_set_dt(&ledMatrixMuxA,(row & 0x1));
		gpio_pin_set_dt(&ledMatrixMuxB,(row & 0x2));
		gpio_pin_set_dt(&ledMatrixMuxC,(row & 0x4));
		gpio_pin_set_dt(&ledMatrixMuxD,(row & 0x8));

		//TODO: determine this k_sleep delay
		k_sleep(K_USEC(1000));
	}
	return 0;
}
