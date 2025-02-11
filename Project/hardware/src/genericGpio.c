#include "genericGpio.h"

/** 
 * @brief Configures the switches.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool switchesConfig()
{
	//check if gpio is available
	uint8_t ret = 0;
	uint8_t amount = 5;
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
	return 0;
}

/** 
 * @brief Configures the abcbuttons.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool abcbuttonsConfig()
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
	return 0;
}

/** 
 * @brief Configures the abcleds.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool abcledsConfig()
{
	//check if gpio is available
	uint8_t ret = 0;
	uint8_t amount = 3;
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_is_ready_dt(&abcleds[i]);
	}
	if(ret != amount)
	{
		return 1;
	}
	ret = 0;

	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_pin_configure_dt(&abcleds[i], GPIO_OUTPUT_ACTIVE);
	}

	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief Configures the button en led for start.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool startbuttonledConfig()
{
	//Checks if gpio is available
	if (!gpio_is_ready_dt(&startbutton) && !gpio_is_ready_dt(&startled)) 
	{
		return 1;
	}
	//configures the gpio
	uint8_t ret = 0;
	ret += gpio_pin_configure_dt(&startled, GPIO_OUTPUT_ACTIVE);
	//return when gpio is configured incorrectly
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}



/** 
 * @brief Initializes the switches
 * 
 * Sets all pins from floating to 1
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t switchesInit()
{
	uint8_t ret = 0;
	uint8_t amount = 5;
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

/** 
 * @brief Initializes the abc buttons
 * 
 * Sets all pins from floating to 1
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t abcbuttonsInit()
{
	uint8_t ret = 0;
	uint8_t amount = 3;
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_pin_configure_dt(&abcbutton[i],GPIO_INPUT);
	}
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief Initializes the abc leds
 * 
 * Sets all pins from floating to 0
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t abcledsInit()
{
	uint8_t ret = 0;
	uint8_t amount = 3;
	for (uint8_t i = 0; i < amount; i++)
	{
		ret += gpio_pin_set_dt(&abcleds[i],LOW);
	}
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}


/** 
 * @brief Initializes the buttonsinsmallmatrix
 * 
 * Sets button pin from floating to 1
 * Sets led pin from floating to 0
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t startbuttonledInit()
{
	uint8_t ret = 0;
	ret += gpio_pin_configure_dt(&startbutton, GPIO_INPUT);
	ret += gpio_pin_set_dt(&startled,LOW);
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}

/** 
 * @brief reads value from switches
 * 
 * per switch the value can be read using this function
 * 
 * @param[in] selectedswitch – select which switch value is returned
 * 
 * @return Returns 0 when the switch is in the on state
 * Returns a 1 when the switch is in the off state
 * Returns a 2 when the switch is in the middle state
 */ 
uint8_t switchesGet(uint8_t selectedswitch)
{
	bool switchonState = false,switchoffState = false;
	switchonState = gpio_pin_get(switchon[selectedswitch].port, switchon[selectedswitch].pin);
	switchoffState = gpio_pin_get(switchoff[selectedswitch].port, switchoff[selectedswitch].pin);

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

/** 
 * @brief reads value from abc buttons
 * 
 * per button the value can be read using this function
 * 
 * @param[in] selectedbtn – select which switch value is returned a char is used to choose a,b or c
 * 
 * @return Returns 0 when the button is in the on state
 * Returns a 1 when the button is in the off state
 * Returns a 2 when trying to access a button that does not exist
 */ 
uint8_t abcbuttonsGet(char selectedbtn)
{
	switch (selectedbtn)
	{
	case 'a':
		return gpio_pin_get(abcbutton[0].port, abcbutton[0].pin);
		break;
	case 'b':
		return gpio_pin_get(abcbutton[1].port, abcbutton[1].pin);
		break;
	case 'c':
		return gpio_pin_get(abcbutton[2].port, abcbutton[2].pin);
		break;
	default:
		return 2;
		break;
	}
}

/** 
 * @brief writes value to abc leds
 * 
 * per button the value can be read using this function
 * 
 * @param[in] selectedled – select which led is set
 * @param[in] value – select what the value will be set to of the selected led
 * 
 * @return Returns 0 when a led is set.
 * Returns a 1 when trying to set a led that does not exist
 */ 
uint8_t abcledsSet(char selectedled,bool value)
{
	switch (selectedled)
	{
	case 'a':
		gpio_pin_set_dt(&abcleds[0],value);
		return 0;
		break;
	case 'b':
		gpio_pin_set_dt(&abcleds[1],value);
		return 0;
		break;
	case 'c':
		gpio_pin_set_dt(&abcleds[2],value);
		return 0;
		break;
	default:
		return 1;
		break;
	}
}

/** 
 * @brief reads value from start button
 * 
 * reads value from start button
 * 
 * 
 * @return Returns 0 when a button is pressed.
 * Returns a 1 when button is not pressed
 */ 
uint8_t startbuttonGet()
{
	return gpio_pin_get(startbutton.port, startbutton.pin);
}

/** 
 * @brief writes value to start led
 * 
 * writes value to start led
 * 
 * @param[in] value – select what the value will be set for the led
 * 
 * @return Always returns a 0
 */ 
uint8_t startledSet(bool value)
{
	gpio_pin_set_dt(&startled,value);
	return 0;
}