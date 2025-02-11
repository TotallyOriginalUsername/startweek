#include "testIntegration.h"
#define INTEGRATION
#ifdef INTEGRATION


// The stack size of all threads
#define STACKSIZE 1024

// Thread priority values (lower value is higher priority)
#define TMAIN_PRIORITY 8
void integration(void);
// Define the threads
K_THREAD_DEFINE(t_integrationid, STACKSIZE, integration, NULL, NULL, NULL, TMAIN_PRIORITY, 0, 0);


#define TIME_INTERVAL 60 	// Time in seconds
#define TEST_DURATION 6		// Time in hours

#define NUM_OBJECTS 6							// Amount of abjects 		DO NOT CHANGE
#define TEST_DURATION_INT	TEST_DURATION*60*60	// Time of hours in seconds	DO NOT CHANGE

// uint8_t startbuttonGetMutexValue();
// uint8_t* btnmatrix_inGetMutexValue();
// uint8_t* switchesGetMutexValue();
// int32_t potmeterGetMutexValue();
// uint8_t* abcbtnGetMutexValue();


void integration(void)
{
	configure();
	initialize();
	//thread values
	uint32_t duration = 0;
	unsigned amount = 9;
	char *names[10] = {"startbtn", "btnmatrix_in", "switches", "potmeter", "abcbtn","btnmatrix_out","ledmatrix","ledcircle","sevenseg"};
	Startupdelay = 0;
	enableThreads(names, amount);

	// Matrix values
	// Initial state with all bits set to 1 (all on)
	uint8_t databtnmatrix_on[4] = {0b00001111, 0b00001111, 0b00001111, 0b00001111};
	uint8_t dataledcircle_on[8] = {0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111};
	uint16_t dataledmatrix_on[16] = {
		0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
		0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
		0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
		0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111
	};

	// Active state with some bits turned off
	uint8_t databtnmatrix_active[4] = {0b11101111, 0b11011111, 0b10111111, 0b01111111};
	uint8_t dataledcircle_active[8] = {0b11111110, 0b11111101, 0b11111011, 0b11110111, 0b11101111, 0b11011111, 0b10111111, 0b01111111};
	uint16_t dataledmatrix_active[16] = {
		0b1111111111111110, 0b1111111111111101, 0b1111111111111011, 0b1111111111110111,
		0b1111111111101111, 0b1111111111011111, 0b1111111110111111, 0b1111111101111111,
		0b1111111011111111, 0b1111110111111111, 0b1111101111111111, 0b1111011111111111,
		0b1110111111111111, 0b1101111111111111, 0b1011111111111111, 0b0111111111111111
	};

	// Off state with all bits set to 0 (all off)
	uint8_t databtnmatrix_off[4] = {0b00000000, 0b00000000, 0b00000000, 0b00000000};
	uint8_t dataledcircle_off[8] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
	uint16_t dataledmatrix_off[16] = {
		0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
		0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
		0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
		0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000
	};

	//sevensegment values
	char inputsevensegment_on[4] = "1234";
	char inputsevensegment_off[4] = "0000";

	//lcd values


	//btnmatrix,ledmatrix,ledcircle,sevenseg,lcd,startled+abcled
    //test values
	int currentIndex = 3;
	//turn on first 3 systems.
	btnmatrix_outSetMutexValue(databtnmatrix_on);
	ledmatrixSetMutexValue(dataledmatrix_on);
	ledcircleSetMutexValue(dataledcircle_on);

	//start looping for 6 hours each system.
	while (duration != TEST_DURATION_INT) {
		printf("currentIndex %d\n",currentIndex);
		switch (currentIndex)
		{
		case 0:
			btnmatrix_outSetMutexValue(databtnmatrix_on);
			sevensegSetMutexValue(inputsevensegment_off,5);
			
			break;
		case 1:
			ledmatrixSetMutexValue(dataledmatrix_on);
			lcdClear();
			lcdDisable();
			break;
		case 2:
			ledcircleSetMutexValue(dataledcircle_on);
			startledSet(0);
			abcledsSet('a',0);
			abcledsSet('b',0);
			abcledsSet('c',0);
			break;
		case 3:
			sevensegSetMutexValue(inputsevensegment_on,1);
			btnmatrix_outSetMutexValue(databtnmatrix_off);
			break;
		case 4:
			lcdEnable();
			lcdStringWrite("Druk op start");
			ledmatrixSetMutexValue(dataledmatrix_off);
			break;
		case 5:
			startledSet(1);
			abcledsSet('a',1);
			abcledsSet('b',1);
			abcledsSet('c',1);
			ledcircleSetMutexValue(dataledcircle_off);
			break;		
		default:
			break;
		}
        k_busy_wait(TIME_INTERVAL*1000000);
        currentIndex = (currentIndex + 1) % NUM_OBJECTS;
		duration++;
    }
	disableThreads(names, amount);
}
#endif //INTEGRATION