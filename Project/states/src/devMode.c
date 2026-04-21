#include "devMode.h"
#include "genericGpio.h"
#include "helperFunctions.h"
#include "idle.h"

unsigned devmodeThreadCount = 1;
char* devmodeThreads[1] = {"abcbtn"};

void getDevModethreads(char ***names, unsigned *amount) {
	*names = devmodeThreads;
	*amount = devmodeThreadCount;
}

// Returns MG ID of next game to try, -1 for going back to idle
int playDevMode(bool* devMode){
	static int testIndex = 1;
	char lcd_msg[32];

	// reset the testindex after leaving devMode
	if(testIndex == -1){
		testIndex = 1;
	}

	lcdEnable();
	// lcdStringWrite("Selecteer een spel met A en C");
	// k_msleep(3000);
	lcdStringWrite("Bevestig met de startknop!");
	k_msleep(3000);

	abcledsSet('a', 1);
	abcledsSet('c', 1);
	startledSet(1);
	sprintf(lcd_msg, "Minigame: %d", testIndex);
	lcdStringWrite(lcd_msg);

	while(startbuttonGet()){
		native_loop();
		wait_till_abc_depressed();

		if(abcbuttonsGet('a') == 0){
			if(testIndex == 1){
				testIndex = 15;
			}
			else{
				testIndex--;
			}
			sprintf(lcd_msg, "Minigame: %d", testIndex);
			lcdStringWrite(lcd_msg);
		} else if (abcbuttonsGet('b') == 0) {
            *devMode = false;
            testIndex = -1;
            break;
        } else if(abcbuttonsGet('c') == 0){
			if(testIndex == 15){
				testIndex = 1;
			}
			else{
				testIndex++;
			}
			sprintf(lcd_msg, "Minigame: %d", testIndex);
			lcdStringWrite(lcd_msg);
		}
	}

	startledSet(0);
	abcledsSet('a', 0);
	abcledsSet('c', 0);
	lcdClear();
	lcdDisable();
	k_msleep(100);

	return testIndex;
}
