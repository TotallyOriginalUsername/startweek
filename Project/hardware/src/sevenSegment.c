#include "sevenSegment.h"


/** 
 * @brief sets one segment.
 * 
 * sets the correct pins to display the value on one segment
 * 
 * @return does not return.
 */ 
void sevenSegmentOneSegment(char value,bool dp)
{
	switch (value)
	{
	case '0':
		gpio_pin_set_dt(&sevenSegmentBcdA,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '1':
		gpio_pin_set_dt(&sevenSegmentBcdA,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '2':
		gpio_pin_set_dt(&sevenSegmentBcdA,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdB,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '3':
		gpio_pin_set_dt(&sevenSegmentBcdA,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdB,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '4':
		gpio_pin_set_dt(&sevenSegmentBcdA,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdC,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '5':
		gpio_pin_set_dt(&sevenSegmentBcdA,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdC,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '6':
		gpio_pin_set_dt(&sevenSegmentBcdA,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdB,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdC,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '7':
		gpio_pin_set_dt(&sevenSegmentBcdA,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdB,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdC,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '8':
		gpio_pin_set_dt(&sevenSegmentBcdA,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdD,HIGH);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	case '9':
		gpio_pin_set_dt(&sevenSegmentBcdA,HIGH);
		gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
		gpio_pin_set_dt(&sevenSegmentBcdD,HIGH);
		gpio_pin_set_dt(&sevenSegmentDP,dp);
		break;
	default:
		break;
	}
}


/** 
 * @brief Configures the sevenSegment.
 * 
 * Checks if the GPIO is available and configures the GPIO for its purpose
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
bool sevenSegmentConfig()
{
	if (!gpio_is_ready_dt(&sevenSegmentBcdA) 	&& !gpio_is_ready_dt(&sevenSegmentBcdB) 	&&
		!gpio_is_ready_dt(&sevenSegmentBcdC) 	&& !gpio_is_ready_dt(&sevenSegmentBcdD) 	&&
		!gpio_is_ready_dt(&sevenSegmentDP) 		&& !gpio_is_ready_dt(&sevenSegmentMuxDig1) 	&&
		!gpio_is_ready_dt(&sevenSegmentMuxDig2) && !gpio_is_ready_dt(&sevenSegmentMuxDig3)	&& 
		!gpio_is_ready_dt(&sevenSegmentMuxDig4)) 
	{
		return 1;
	}
	//configures the gpio
	int ret = 0;
	ret += gpio_pin_configure_dt(&sevenSegmentBcdA, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentBcdB, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentBcdC, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentBcdD, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentDP, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentMuxDig1, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentMuxDig2, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentMuxDig3, GPIO_OUTPUT_ACTIVE);
	ret += gpio_pin_configure_dt(&sevenSegmentMuxDig4, GPIO_OUTPUT_ACTIVE);
	//return when gpio is configured incorrectly
	if (ret != 0) 
	{
		return 1;
	}
	return 0;
}


/** 
 * @brief Initializes the sevenSegment
 * 
 * Sets all pins from floating to 0
 * Clears the value by writing 0
 * 
 * @return Returns a 0 on succes and a 1 on error.
 */ 
uint8_t sevenSegmentInit()
{
	//Checks if gpio is available
	
	uint8_t ret = 0;
	ret += gpio_pin_set_dt(&sevenSegmentBcdA,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentBcdB,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentBcdC,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentBcdD,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentDP,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentMuxDig1,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentMuxDig2,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentMuxDig3,LOW);
	ret += gpio_pin_set_dt(&sevenSegmentMuxDig4,LOW);
	k_sleep(K_MSEC(5));
	if(ret != 0)
	{
		return 1;
	}

	return 0;
}



/** 
 * @brief write a value with one decimalpoint to the sevenSegment
 * 
 * can write 4 char value to lcd and show one decimal point.
 * 
 * @return Returns a 0 
 */ 
uint8_t sevenSegmentSet(char input[4],uint8_t dpPosition)
{
	for (size_t i = 0; i < 4; i++)
	{
		bool dp = false;
		switch (i)
		{
		case 0:
			gpio_pin_set_dt(&sevenSegmentMuxDig1,HIGH);
			gpio_pin_set_dt(&sevenSegmentMuxDig2,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig3,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig4,LOW);
			break;
		case 1:
			gpio_pin_set_dt(&sevenSegmentMuxDig1,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig2,HIGH);
			gpio_pin_set_dt(&sevenSegmentMuxDig3,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig4,LOW);
			break;
		case 2:
			gpio_pin_set_dt(&sevenSegmentMuxDig1,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig2,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig3,HIGH);
			gpio_pin_set_dt(&sevenSegmentMuxDig4,LOW);
			break;
		case 3:
			gpio_pin_set_dt(&sevenSegmentMuxDig1,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig2,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig3,LOW);
			gpio_pin_set_dt(&sevenSegmentMuxDig4,HIGH);
			break;
		}
		if(dpPosition == i)
		{
			dp = true;
		}
		sevenSegmentOneSegment(input[i],dp);
		//needs to be determined
		k_msleep(4);
	}
	return 0;
}