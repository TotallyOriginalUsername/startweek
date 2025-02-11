#include "configure.h"

/** 
 * @brief Configures all hardware for system
 * 
 * Configures all hardware for system. defining output, and input
 * 
 * @return Returns 0 on succes and > 0 on amount of failures
 */ 
uint8_t configure()
{
	uint8_t ret = 0;
	ret += buttonMatrixConfig();
	ret += circleMatrixConfig();
	ret += buttons4x4Config();
	ret += switchesConfig();
	ret += abcbuttonsConfig();
	ret += abcledsConfig();
	ret += startbuttonledConfig();
	ret += lcdConfig();
	ret += ledMatrixConfig();
	ret += sevenSegmentConfig();
	return ret;
}