#ifndef IDLE_H
#define IDLE_H
#include <stdbool.h>
#include <stdint.h>

void reloadLocations();
void reloadProgress();

int getLocationAmount();

void getIdleThreads(char ***, unsigned *);
int playIdle(uint8_t* trivia_ID, bool* devModeOn);
void walkToEndLocation();

#endif // IDLE_H