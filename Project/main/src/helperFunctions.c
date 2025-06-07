#include "helperFunctions.h"

LOG_MODULE_REGISTER(helperFunctions);
K_TIMER_DEFINE(timer, NULL, NULL);

// LED circle direction to location related definitions
#define ANGLE_BUFFER_SIZE 10 // Number of angles to average for the circle direction
#define DIST_MAX_WIDTH 20	// Distance at which the circle has minimum width
#define DIST_MIN_WIDTH 200 	//Distance at which the circle has maximum width
#define DIST_RANGE (DIST_MIN_WIDTH - DIST_MAX_WIDTH)
#define LEDS_MIN_WIDTH 3	// Amount of LEDS at DIST_MIN_WIDTH
#define LEDS_MAX_WIDTH 48	// Amount of LEDS at DIST_MAX_WIDTH
#define LEDS_RANGE_WIDTH (LEDS_MAX_WIDTH - LEDS_MIN_WIDTH)
#define MAX_DIR 359
#define NR_OF_LEDS 64


//Convert btnmatrix input to btnmatrix output format
void btnmatrix_in_to_out(uint8_t* btnmatrix_input, uint8_t* btnmatrix_output){
	for (uint8_t i = 0; i < 16; i++) {
        if (!btnmatrix_input[i]) {
            uint8_t row = i / 4;
#ifdef CONFIG_ARCH_POSIX
            uint8_t column = i % 4;
#else
            uint8_t column = 3 - (i % 4);
#endif
            btnmatrix_output[row] |= (1 << column);
        }
    }
}

//Convert btnmatrix output to ledmatrix format
void btnmatrix_to_ledmatrix(uint8_t* btnmatrix_shape, uint16_t* ledmatrix_shape){

	for (int row = 0; row < 4; row++) {
        uint16_t ledmatrix_row = 0;

        for (int bit = 0; bit < 4; bit++) {
            if (btnmatrix_shape[row] & (1 << bit)) {
                ledmatrix_row |= (0b1111 << (bit * 4));
            }
        }

        ledmatrix_shape[row * 4] = ledmatrix_row;
        ledmatrix_shape[row * 4 + 1] = ledmatrix_row;
        ledmatrix_shape[row * 4 + 2] = ledmatrix_row;
        ledmatrix_shape[row * 4 + 3] = ledmatrix_row;
    }
}

//clears the btnmatrix led matrix
void clear_btnmatrix_leds(){
	uint8_t data_button_matrix[4] = {0};
	btnmatrix_outSetMutexValue(data_button_matrix);
}

//function to show a circle on the button matrix for 2 seconds
void show_correct(){
	uint8_t data_button_matrix[4] = {0b00000110, 0b00001001, 0b00001001, 0b00000110};
	btnmatrix_outSetMutexValue(data_button_matrix);
	k_msleep(2000);
	clear_btnmatrix_leds();
}

//function to show a cross on the button matrix  for 2 seconds
void show_incorrect(){
	uint8_t data_button_matrix[4] = {0b00001001, 0b00000110, 0b00000110, 0b00001001};
	btnmatrix_outSetMutexValue(data_button_matrix);
	k_msleep(2000);
	clear_btnmatrix_leds();
}

void show_oneliners(char input_array[][32], int lines)
{
	lcdEnable();

	for (uint8_t i = 0; i < lines; i++){
		lcdStringWrite(input_array[i]);
		k_msleep(3000);
	}

	lcdClear();
	lcdDisable();
}

//Function to wait untill every abc button has been released
void wait_till_abc_depressed(){
    uint8_t input_count = 1;
    uint8_t *abcbtns;
    LOG_WRN("Waiting\n");

    while(input_count != 0){
        abcbtns = abcbtnGetMutexValue();
        input_count = 0;

        for (int i = 0; i < 3; i++){
			if (abcbtns[i] == 0){
				input_count++;
			}
		}
		native_loop();
    }

    LOG_WRN("Done waiting\n");
}

//Function to wait untill every 4x4 button has been released
void wait_till_btnmatrix_depressed(){
    uint8_t input_count = 1;
    uint8_t *btnmatrix;
    LOG_WRN("Waiting\n");

    while(input_count != 0){
        btnmatrix = btnmatrix_inGetMutexValue();
        input_count = 0;

        for (int i = 0; i < 16; i++){
			if (btnmatrix[i] == 0){
				input_count++;
			}
		}
		native_loop();
    }

    LOG_WRN("Done waiting\n");
}

//Function to wait untill every switch is at rest
void wait_till_switches_rest(){
    uint8_t input_count = 1;
    uint8_t *switches;
    LOG_WRN("Waiting\n");

    while(input_count != 0){
        switches = switchesGetMutexValue();
        input_count = 0;

        for (int i = 0; i < 5; i++){
			if (switches[i] == 1){
				input_count++;
			}
		}
		native_loop();
    }

    LOG_WRN("Done waiting\n");
}

//Get functions
//wait till the input time has been reached or one abc button has been pressed
void get_abc_input(uint16_t input_time, uint8_t* input_array, size_t size){
	uint8_t input_count = 0;

	k_timer_start(&timer, K_MSEC(input_time), K_NO_WAIT);
	LOG_WRN("5 sec wait\n");
	while ((!(k_timer_status_get(&timer) > 0)) && (input_count == 0)){
		memcpy(input_array, abcbtnGetMutexValue(), size);
		for (int i = 0; i < 3; i++){
			if (input_array[i] == 0){
				input_count++;
			}
		}
		native_loop();
	}
	LOG_WRN("Done waiting\n");
}

//wait till the input time has been reached or one btnmatrix button has been pressed
void get_btnmatrix_input(uint16_t input_time, uint8_t* input_array, size_t size){
	uint8_t input_count = 0;

	k_timer_start(&timer, K_MSEC(input_time), K_NO_WAIT);
	LOG_WRN("5 sec wait\n");
	while ((!(k_timer_status_get(&timer) > 0)) && (input_count == 0)){
		memcpy(input_array, btnmatrix_inGetMutexValue(), size);
		for (int i = 0; i < 16; i++){
			if (input_array[i] == 0){
				input_count++;
			}
		}
		native_loop();
	}
	LOG_WRN("Done waiting\n");
}

//Returns the pressed button, or 17 upon failure
uint8_t get_btnmatrix_input_number(uint16_t input_time){
    uint8_t input_array[16] = {1};
    k_timer_start(&timer, K_MSEC(input_time), K_NO_WAIT);

    while ((!(k_timer_status_get(&timer) > 0))){
        memcpy(input_array, btnmatrix_inGetMutexValue(), sizeof(input_array));
		for (int i = 0; i < 16; i++){
			if (input_array[i] == 0){
				return i;
			}
		}
		native_loop();
	}
	return 17;
}

//Returns the pressed button, or 17 upon failure
uint8_t get_btnmatrix_input_number_untimed(){
	uint8_t input = 0;
    uint8_t input_array[16];

    while (input == 0){
        memcpy(input_array, btnmatrix_inGetMutexValue(), sizeof(input_array));
		for (int i = 0; i < 16; i++){
			if (input_array[i] == 0){
				input = 1;
				return i;
			}
		}
		k_sleep(K_MSEC(1));
	}
	return 17;
}

//Set functions

//Set one led in the btnmatrix led matrix
void set_btnmatrix_led(uint8_t position){
	uint8_t data_button_matrix[4] = {0};

	uint8_t row = position / 4;
#ifdef CONFIG_ARCH_POSIX
	uint8_t column = position % 4;
#else
	uint8_t column = 3 - (position % 4);
#endif

	data_button_matrix[row] |= (1 << column);

	btnmatrix_outSetMutexValue(data_button_matrix);
}

/**
 * @brief Convert degrees to radians.
 * @param degree The angle in degrees.
 * @return The angle in radians.
 */
int get_led_width(int distance) {
	if (distance > DIST_MIN_WIDTH) { return LEDS_MIN_WIDTH; }
	if (distance < DIST_MAX_WIDTH) { return LEDS_MAX_WIDTH; }
	distance -= DIST_MAX_WIDTH;
	double distFraction = (double)distance / DIST_RANGE;
	distFraction = 1 - distFraction;
	return LEDS_RANGE_WIDTH*distFraction + LEDS_MIN_WIDTH;
}

/**
 * @brief Convert degrees to radians.
 * @param degree The angle in degrees.
 * @return The angle in radians.
 */
int circleMovingAvg(int newValue) {
	double rad = toRadians(newValue);
	double sinVal = sin(rad);
	double cosVal = cos(rad);

	static double angleSinBuffer[ANGLE_BUFFER_SIZE];
	static double angleCosBuffer[ANGLE_BUFFER_SIZE];

	for (int i = ANGLE_BUFFER_SIZE-1; i > 0; i--) {	// Move every entry in the buffer up by one index
		angleSinBuffer[i] = angleSinBuffer[i-1];
		angleCosBuffer[i] = angleCosBuffer[i-1];
	}
	angleSinBuffer[0] = sinVal;						// Insert the new value
	angleCosBuffer[0] = cosVal;

	double sinSum = 0;
	double cosSum = 0;
	for (int i = 0; i < ANGLE_BUFFER_SIZE; i++) {
		sinSum += angleSinBuffer[i];
		cosSum += angleCosBuffer[i];
	}
	double sinAvg = sinSum / ANGLE_BUFFER_SIZE;
	double cosAvg = cosSum / ANGLE_BUFFER_SIZE;
	return toDegrees(atan2(sinAvg,cosAvg));
}

/**
 * @brief Function to set the led circle direction and distance for showing where the user should go next.
 * @param dir The direction in degrees (0-359) where the LEDs should be centered.
 * @param distance The distance in meters to the target location, which will determine the width of the LED circle.
 */
void set_led_circle_dir_dist(int dir, int distance)
{
	int width = get_led_width(distance); // Convert distance to width in pixels

	float centerFloat = (float)dir / MAX_DIR;
	int centerPixel = round(centerFloat * NR_OF_LEDS);

	int leftBound = centerPixel - (width / 2);
	if (leftBound < 0 ) {
		leftBound = NR_OF_LEDS + leftBound;
	}
	int rightBound = centerPixel + (width / 2);
	if (rightBound > (NR_OF_LEDS - 1)) {
		rightBound = rightBound - NR_OF_LEDS;
	}
	bool overlap = false;
	if (leftBound > rightBound) {
		overlap = true;
	}

	const int nrBytes = 8;
	const int nrBits = 8;
	uint8_t outputValues[8] = {0,0,0,0,0,0,0,0};	// Initialize to zero (all LEDs off)
	for (int byteCount = 0; byteCount < nrBytes; byteCount++) {
		for (int bitCount = 0; bitCount < nrBits; bitCount++) {
			int bitIndex = 8 * byteCount + bitCount;
			//bool enableLed = (!overlap && (bitIndex >= leftBound) && (bitIndex >= rightBound)) || (overlap && (bitIndex >= leftBound || bitIndex <= rightBound));
			bool enableLed = false;
			if (!overlap) {
				if (bitIndex >= leftBound && bitIndex <= rightBound) {
					enableLed = true;
				}
			} else {
				if (bitIndex >= leftBound || bitIndex <= rightBound) {
					enableLed = true;
				}
			}
			//LOG_WRN("Byte %d bit %d (bitindex %d): %d\n", byteCount, bitCount, bitIndex, enableLed);
			if (enableLed) {
				outputValues[byteCount] |= (1 << (7-bitCount));
			}

		}
	}
	ledcircleSetMutexValue(outputValues);
}

/**
 * @brief Convert degrees to radians.
 * @param dir The angle in degrees.
 * @return The angle in radians.
 */
int get_relative_dir(int dir)
{
	int compassDir;
	gyroCompass_get_heading(&compassDir);	// Angle of device
	compassDir = circleMovingAvg(compassDir);
	dir = dir - compassDir;					// Add to get the direction compared to the device
	if (dir < 0 ) {
		dir += 360;
	} else if (dir >= 360)
	{
		dir -= 360;
	}
	return dir;	// Return the direction relative to the device
}