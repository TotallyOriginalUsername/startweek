#include "buttonMatrix.h"

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
void buttonMatrixSendOneBitData(bool ShiftDataValue)
{
	gpio_pin_set_dt(&buttonMatrixShiftDataIn,ShiftDataValue);
	gpio_pin_set_dt(&buttonMatrixShiftClock,HIGH);
	gpio_pin_set_dt(&buttonMatrixShiftClock,LOW);
}

/** 
 * @brief Configures the buttonMatrix.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool buttonMatrixConfig()
{
	//Checks if gpio is available
	if (!gpio_is_ready_dt(&buttonMatrixShiftDataIn) && !gpio_is_ready_dt(&buttonMatrixShiftOutputEnable) &&
		!gpio_is_ready_dt(&buttonMatrixShiftClock) && !gpio_is_ready_dt(&buttonMatrixMuxA) &&
		!gpio_is_ready_dt(&buttonMatrixMuxB)) 
	{
		return 1;
	}
	//configures the gpio
	uint8_t ret = 0;
	ret += gpio_pin_configure_dt(&buttonMatrixShiftDataIn, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&buttonMatrixShiftOutputEnable, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&buttonMatrixShiftClock, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&buttonMatrixMuxA, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&buttonMatrixMuxB, GPIO_OUTPUT_ACTIVE);
	//return when gpio is configured incorrectly
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief Initializes the buttonMatrix
 * 
 * Sets all pins from floating to 0
 * Clears the Shift registers by writing 16 times a 0.
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t buttonMatrixInit ()
{
	uint8_t ret = 0;
	ret += gpio_pin_set_dt(&buttonMatrixShiftDataIn,LOW);
	ret += gpio_pin_set_dt(&buttonMatrixShiftOutputEnable,LOW);
	ret += gpio_pin_set_dt(&buttonMatrixShiftClock,LOW);
	ret += gpio_pin_set_dt(&buttonMatrixMuxA,LOW);
	ret += gpio_pin_set_dt(&buttonMatrixMuxB,LOW);

	if (ret != 0) 
	{
		return 1;
	}
	for (size_t row = 0; row < BUTTONMATRIXROWS; row++)
	{
		for (int i = 0; i < BUTTONMATRIXLEDSINROW; i++)
		{
			buttonMatrixSendOneBitData(LOW);
		}
		gpio_pin_set_dt(&buttonMatrixShiftOutputEnable,HIGH);
		gpio_pin_set_dt(&buttonMatrixShiftOutputEnable,LOW);

		gpio_pin_set_dt(&buttonMatrixMuxA,(row & 0x1));
		gpio_pin_set_dt(&buttonMatrixMuxB,(row & 0x2));
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
uint8_t buttonMatrixSet(uint8_t data[BUTTONMATRIXROWS])
{
	for (size_t row = 0; row < BUTTONMATRIXROWS; row++)
	{
		for (size_t led = 0; led < BUTTONMATRIXLEDSINROW; led++)
		{
			if(data[row] & 0x1<<led)
			{
				buttonMatrixSendOneBitData(HIGH);
			}
			else
			{
				buttonMatrixSendOneBitData(LOW);
			}
		}

		gpio_pin_set_dt(&buttonMatrixShiftOutputEnable,HIGH);
		gpio_pin_set_dt(&buttonMatrixShiftOutputEnable,LOW);

		gpio_pin_set_dt(&buttonMatrixMuxA,(row & 0x1));
		gpio_pin_set_dt(&buttonMatrixMuxB,(row & 0x2));

		//TODO: determine this k_sleep delay
		k_sleep(K_USEC(4000));
	}
	return 0;
}


/** 
 * @brief Configures the buttonsinsmallmatrix.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool buttons4x4Config()
{
	//check if gpio is available
	uint8_t ret = 0;
	uint8_t amount = 16;
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_is_ready_dt(&buttonsButtonMatrix[i]);
	}
	if(ret != amount)
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief Initializes the buttonsinsmallmatrix
 * 
 * Sets all pins from floating to 1
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t buttons4x4Init()
{
	uint8_t ret = 0;
	uint8_t amount = 16;
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_pin_configure_dt(&buttonsButtonMatrix[i],GPIO_INPUT);
	}
	if (ret != 0) {
		return 1;
	}
	return 0;
}


/** 
 * @brief reads value from buttons in small matrix
 * 
 * per button the value can be read using this function
 * 
 * @param[in] selectedbtn – select which button value is returned
 * 
 * @return Returns 0 when the button is pressed
 * Returns a 1 if the button is not pressed
 * Returns a 2 when trying to access a button that does not exist
 */ 
uint8_t buttons4x4Get(uint8_t selectedbtn)
{	
	if(selectedbtn < 16 && selectedbtn >= 0)
	{
		return gpio_pin_get(buttonsButtonMatrix[selectedbtn].port, buttonsButtonMatrix[selectedbtn].pin);
	}
	else
	{
		return 2;
	}
	
}