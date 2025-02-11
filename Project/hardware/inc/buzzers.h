#ifndef BUZZERS_h
#define BUZZERS_h
#include <stdio.h>

uint8_t buzzersInit();
uint8_t buzzerSetPwm(int aBuzzerNum, int aFreq);
uint8_t buzzerTurnOff(int aBuzzerNum);
uint8_t buzzersExit();


#endif /* BUZZERS_h */
