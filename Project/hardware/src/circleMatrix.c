
#include "circleMatrix.h"

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
void circleMatrixSendOneBitData(bool ShiftDataValue)
{
	gpio_pin_set_dt(&circleMatrixShiftDataIn,ShiftDataValue);
	gpio_pin_set_dt(&circleMatrixShiftClock,HIGH);
	gpio_pin_set_dt(&circleMatrixShiftClock,LOW);
}

/** 
 * @brief Configures the circleMatrix.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool circleMatrixConfig()
{
	//Checks if gpio is available
	if (!gpio_is_ready_dt(&circleMatrixShiftDataIn) && !gpio_is_ready_dt(&circleMatrixShiftOutputEnable) &&
		!gpio_is_ready_dt(&circleMatrixShiftClock) && !gpio_is_ready_dt(&circleMatrixMuxA) &&
		!gpio_is_ready_dt(&circleMatrixMuxB) && !gpio_is_ready_dt(&circleMatrixMuxC)) 
	{
		return 1;
	}
	//configures the gpio
	uint8_t ret = 0;
	ret += gpio_pin_configure_dt(&circleMatrixShiftDataIn, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&circleMatrixShiftOutputEnable, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&circleMatrixShiftClock, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&circleMatrixMuxA, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&circleMatrixMuxB, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&circleMatrixMuxC, GPIO_OUTPUT_ACTIVE);
	//return when gpio is configured incorrectly
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief Initializes the circleMatrix
 * 
 * Sets all pins from floating to 0
 * Clears the Shift registers by writing 16 times a 0.
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t circleMatrixInit ()
{
	uint8_t ret = 0;
	ret += gpio_pin_set_dt(&circleMatrixShiftDataIn,LOW);
	ret += gpio_pin_set_dt(&circleMatrixShiftOutputEnable,LOW);
	ret += gpio_pin_set_dt(&circleMatrixShiftClock,LOW);
	ret += gpio_pin_set_dt(&circleMatrixMuxA,LOW);
	ret += gpio_pin_set_dt(&circleMatrixMuxB,LOW);
	ret += gpio_pin_set_dt(&circleMatrixMuxC,LOW);

	if (ret != 0) 
	{
		return 1;
	}
	for (size_t row = 0; row < CIRCLEMATRIXROWS; row++)
	{
		for (int i = 0; i < CIRCLEMATRIXLEDSINROW; i++)
		{
			circleMatrixSendOneBitData(LOW);
		}
		gpio_pin_set_dt(&circleMatrixShiftOutputEnable,HIGH);
		gpio_pin_set_dt(&circleMatrixShiftOutputEnable,LOW);

		gpio_pin_set_dt(&circleMatrixMuxA,(row & 0x1));
		gpio_pin_set_dt(&circleMatrixMuxB,(row & 0x2));
		gpio_pin_set_dt(&circleMatrixMuxC,(row & 0x4));
	}

	return 0;
}

/** 
 * @brief Writes data to the whole matrix
 * 
 * By reading the values from the data variable each bit is defined and will be turned on in te correct sequence.
 * Each row is shifted into a registers and after each row the correct Mux is enabled to select the correct row.
 * 
 * @param[in] data – Array of int8_t with the size of ROWS. Every bit is equal to one led in the matrix.
 * 
 * @return Returns 0 if Configuration and initialization have been performs.
 * Returns a 1 if the Configuration has not been perfomed.
 * Returns a 2 if the Initialiazation has not been perfomed.
 * Returns a 3 if the Configuration and the Initialiazation has not been perfomed.
 */ 
uint8_t circleMatrixSet(uint8_t data[CIRCLEMATRIXROWS])
{
	for (size_t row = 0; row < CIRCLEMATRIXROWS; row++)
	{
		for (size_t led = 0; led < CIRCLEMATRIXLEDSINROW; led++)
		{
			if(data[row] & 0x1<<led)
			{
				circleMatrixSendOneBitData(HIGH);
			}
			else
			{
				circleMatrixSendOneBitData(LOW);
			}
		}

		gpio_pin_set_dt(&circleMatrixShiftOutputEnable,HIGH);
		gpio_pin_set_dt(&circleMatrixShiftOutputEnable,LOW);

		gpio_pin_set_dt(&circleMatrixMuxA,(row & 0x1));
		gpio_pin_set_dt(&circleMatrixMuxB,(row & 0x2));
		gpio_pin_set_dt(&circleMatrixMuxC,(row & 0x4));

		//TODO: determine this k_sleep delay
		k_sleep(K_USEC(2000));
	}
	return 0;
}
