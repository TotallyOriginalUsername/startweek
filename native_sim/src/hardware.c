#include "hardware.h"
#include "lvgl_ui.h"
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

/** 
 * @brief reads value from (LVGL)buttons in small matrix
 * 
 * per button the value can be read using this function
 * 
 * @param[in] selectedbtn – select which button value is returned
 * 
 * @return Returns 0 when the (LVGL)button is pressed
 * Returns a 1 if the (LVGL)button is not pressed
 * Returns a 2 when trying to access a button that does not exist
 */ 
uint8_t buttons4x4GetLVGL(uint8_t selectedbtn)
{	
    uint8_t lvgl_state = 2;
	if(selectedbtn < 16 && selectedbtn >= 0)
	{
        lvgl_state = get_button_state(selectedbtn);
        if(lvgl_state == 1){
            return 1;
        }
		return gpio_pin_get(buttonsButtonMatrix[selectedbtn].port, buttonsButtonMatrix[selectedbtn].pin);
	}
	else
	{
		return 2;
	}
	
}

int8_t ledMatrixSet(int16_t data[LEDMATRIXROWS])
{
	for (size_t row = 0; row < LEDMATRIXROWS; row++)
	{
		for (size_t led = 0; led < LEDMATRIXLEDSINROW; led++)
		{
			int index = row * LEDMATRIXLEDSINROW + led;
			if(data[row] & 0x1<<led)
			{
				set_led((index), 1);
			}
			else
			{
				set_led((index), 0);
			}
		}
	}
	return 0;
}