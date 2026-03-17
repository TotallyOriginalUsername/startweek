#ifndef IDLE_H
#define IDLE_H
#include <stdint.h>

void reloadLocations();

void getIdleThreads(char ***, unsigned *);
int playIdle(uint8_t* trivia_ID);

#endif // IDLE_H