#include "initialize.h"

/** 
 * @brief initializes all hardware for system
 * 
 * initializes all hardware for system. making pings low or high from floating
 * 
 * @return Returns 0 on succes and > 0 on amount of failures
 */ 
uint8_t initialize()
{
	uint8_t ret = 0;
	ret += buttonMatrixInit();
	ret += circleMatrixInit();
	ret += buttons4x4Init();
	ret += switchesInit();
	ret += abcbuttonsInit();
	ret += abcledsInit();
	ret += startbuttonledInit();
	ret += lcdInit();
	ret += ledMatrixInit();
	ret += sevenSegmentInit();
	ret += magnetometer_init();
	ret += gyroscope_init();
	return ret;
}