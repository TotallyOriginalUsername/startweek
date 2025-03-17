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

uint8_t abcbuttonsInit()
{
	//check if gpio is available
	uint8_t ret = 0;
	uint8_t amount = 3;
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_is_ready_dt(&abcbutton[i]);
	}
	if(ret != amount)
	{
		return 1;
	}

	for (uint8_t j = 0; j < amount; j++)
	{
		ret += gpio_pin_configure_dt(&abcbutton[j],GPIO_INPUT);
	}
	if (ret != 0) 
	{
		return 1;
	}

	return 0;
}

uint8_t switchesInit(){
	uint8_t ret = 0;
	uint8_t amount = 5;
	//check if gpio is available
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_is_ready_dt(&switchon[i]);
	}
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_is_ready_dt(&switchoff[i]);
	}
	if(ret != (amount*2))
	{
		return 1;
	}

	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_pin_configure_dt(&switchon[i],GPIO_INPUT);
	}
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_pin_configure_dt(&switchoff[i],GPIO_INPUT);
	}
	if (ret != 0) {
		return 1;
	}

	return 0;
}
// Get functions

uint8_t abcbuttonsGet(char selectedbtn)
{	
    uint8_t lvgl_state = 2;

	switch (selectedbtn)
	{
	case 'a':
		lvgl_state = get_button_abc_state(selectedbtn);
        if(lvgl_state == 1){
            return 1;
        }
		return gpio_pin_get(abcbutton[0].port, abcbutton[0].pin);
		break;
	case 'b':
		lvgl_state = get_button_abc_state(selectedbtn);
        if(lvgl_state == 1){
            return 1;
        }
		return gpio_pin_get(abcbutton[1].port, abcbutton[1].pin);
		break;
	case 'c':
		lvgl_state = get_button_abc_state(selectedbtn);
        if(lvgl_state == 1){
            return 1;
        }
		return gpio_pin_get(abcbutton[2].port, abcbutton[2].pin);
		break;
	default:
		return 2;
		break;
	}
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
		printk("Button %d state: %d", selectedbtn, lvgl_state);
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
/*
 * @return Returns 0 when the switch is in the on state
 * Returns a 1 when the switch is in the off state
 * Returns a 2 when the switch is in the middle state
 */ 
uint8_t switchesGet(uint8_t selectedswitch)
{
	uint8_t lvgl_state = 2;
	bool switchonState = false,switchoffState = false;
	switchonState = gpio_pin_get(switchon[selectedswitch].port, switchon[selectedswitch].pin);
	switchoffState = gpio_pin_get(switchoff[selectedswitch].port, switchoff[selectedswitch].pin);

	lvgl_state = get_switch(selectedswitch);
    if(lvgl_state == 1){
        return 1;
    }

	if(!switchonState)
	{
		return 0;		
	}
	else if(!switchoffState)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

uint8_t startbuttonGet()
{	
	uint8_t lvgl_state = 2;
	lvgl_state = get_button_start();
    if(lvgl_state == 1){
        return 1;
    }
	return gpio_pin_get(startbutton.port, startbutton.pin);
}

// Set functions
uint8_t abcledsSet(char selectedled,bool value)
{
	switch (selectedled)
	{
	case 'a':
		set_abc_button(0,value);
		return 0;
		break;
	case 'b':
		set_abc_button(1,value);
		return 0;
		break;
	case 'c':
		set_abc_button(2,value);
		return 0;
		break;
	default:
		return 1;
		break;
	}
}

uint8_t buttonMatrixSet(uint8_t data[BUTTONMATRIXROWS])
{
	for (size_t row = 0; row < BUTTONMATRIXROWS; row++)
	{
		for (size_t led = 0; led < BUTTONMATRIXLEDSINROW; led++)
		{
			int index = row * BUTTONMATRIXLEDSINROW + led;
			if(data[row] & 0x1<<led)
			{
				set_button((index), 1);
			}
			else
			{
				set_button((index), 0);
			}
		}
		//TODO: determine this k_sleep delay
		k_sleep(K_USEC(4000));
	}
	return 0;
}

uint8_t circleMatrixSet(uint8_t data[CIRCLEMATRIXROWS])
{
	for (size_t row = 0; row < CIRCLEMATRIXROWS; row++)
	{
		for (size_t led = 0; led < CIRCLEMATRIXLEDSINROW; led++)
		{
			int index = row * CIRCLEMATRIXLEDSINROW + led;
			if(data[row] & 0x1<<led)
			{
				set_led_circle((index), 1);
			}
			else
			{
				set_led_circle((index), 0);
			}
		}

		//TODO: determine this k_sleep delay
		k_sleep(K_USEC(2000));
	}
	return 0;
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
		//TODO: determine this k_sleep delay
		//removed from native_sim untill its determined
		//k_sleep(K_USEC(1000));
	}
	return 0;
}

uint8_t sevenSegmentSet(char input[4],uint8_t dpPosition)
{
	set_segmented_display(input, dpPosition);
	//needs to be determined
	k_msleep(4);

	return 0;
}

uint8_t startledSet(bool value)
{
	set_start_button(value);
	return 0;
}

uint8_t lcdStringWrite(char *msg)
{
	set_lcd_display(msg);

	return 0;
}