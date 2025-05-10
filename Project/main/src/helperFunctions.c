#include "helperFunctions.h"

K_TIMER_DEFINE(timer, NULL, NULL);

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
    printk("Waiting\n");

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

    printk("Done waiting\n");
}

//Function to wait untill every 4x4 button has been released
void wait_till_btnmatrix_depressed(){
    uint8_t input_count = 1;
    uint8_t *btnmatrix;
    printk("Waiting\n");

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

    printk("Done waiting\n");
}

//Function to wait untill every switch is at rest
void wait_till_switches_rest(){
    uint8_t input_count = 1;
    uint8_t *switches;
    printk("Waiting\n");

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

    printk("Done waiting\n");
}

//Get functions
//wait till the input time has been reached or one abc button has been pressed
void get_abc_input(uint16_t input_time, uint8_t* input_array, size_t size){
	uint8_t input_count = 0;

	k_timer_start(&timer, K_MSEC(input_time), K_NO_WAIT);
	printk("5 sec wait\n");
	while ((!(k_timer_status_get(&timer) > 0)) && (input_count == 0)){
		memcpy(input_array, abcbtnGetMutexValue(), size);
		for (int i = 0; i < 3; i++){
			if (input_array[i] == 0){
				input_count++;
			}
		}
		native_loop();
	}
	printk("Done waiting\n");
}

//wait till the input time has been reached or one btnmatrix button has been pressed
void get_btnmatrix_input(uint16_t input_time, uint8_t* input_array, size_t size){
	uint8_t input_count = 0;

	k_timer_start(&timer, K_MSEC(input_time), K_NO_WAIT);
	printk("5 sec wait\n");
	while ((!(k_timer_status_get(&timer) > 0)) && (input_count == 0)){
		memcpy(input_array, btnmatrix_inGetMutexValue(), size);
		for (int i = 0; i < 16; i++){
			if (input_array[i] == 0){
				input_count++;
			}
		}
		native_loop();
	}
	printk("Done waiting\n");
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