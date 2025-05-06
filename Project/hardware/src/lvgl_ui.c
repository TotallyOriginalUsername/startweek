#include "lvgl_ui.h"

#if defined(CONFIG_ARCH_POSIX)
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app, CONFIG_LOG_DEFAULT_LEVEL);

#define MAIN_WIDTH 520
#define MAIN_HEIGHT 520

lv_obj_t *buttons[15];
lv_obj_t *buttons_abc[3];
lv_obj_t *button_start;
lv_obj_t *leds[256];
lv_obj_t *leds_circle[64];
lv_obj_t *lcd_display_label;
lv_obj_t *segmented_display_label;
lv_obj_t *switches[5];

// Set functions
void set_abc_button(int button, bool value){
    if(value == 1){
        lv_obj_set_style_bg_color(buttons_abc[button], lv_color_hex(0x00FFFF), 0);
    }
    else if(value == 0){
        lv_obj_set_style_bg_color(buttons_abc[button], lv_color_hex(0xFFFFFF), 0);
    }
    else{
        printk("set_abc_button received an improper state");
    }
}

void set_start_button(bool value){
    if(value == 1){
        lv_obj_set_style_bg_color(button_start, lv_color_hex(0x00FFFF), 0);
    }
    else if(value == 0){
        lv_obj_set_style_bg_color(button_start, lv_color_hex(0xFFFFFF), 0);
    }
    else{
        printk("set_start_button received an improper state");
    }
}


void set_button(int button, int state){
    if(state == 1){
        lv_obj_set_style_bg_color(buttons[button], lv_color_hex(0x00FFFF), 0);
    }
    else if(state == 0){
        lv_obj_set_style_bg_color(buttons[button], lv_color_hex(0xFFFFFF), 0);
    }
    else{
        printk("set_button received an improper state");
    }
}

void set_lcd_display(char *msg){
    lv_label_set_text(lcd_display_label, msg);
}

void set_led(int led, int state){
    if(state == 1){
        lv_obj_set_style_bg_color(leds[led], lv_color_hex(0x00FFFF), 0);
    }
    else if(state == 0){
        lv_obj_set_style_bg_color(leds[led], lv_color_hex(0x000000), 0);
    }
    else{
        printk("set_led received an improper state");
    }
}

void set_led_circle(int led, int state){
    if(state == 1){
        lv_obj_set_style_bg_color(leds_circle[led], lv_color_hex(0x00FFFF), 0);
    }
    else if(state == 0){
        lv_obj_set_style_bg_color(leds_circle[led], lv_color_hex(0x000000), 0);
    }
    else{
        printk("set_led_circle received an improper state");
    }
}

void set_segmented_display(char input[4],uint8_t dpPosition){
    lv_label_set_text(segmented_display_label, input);
}

static void button_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t * target = lv_event_get_target(e);

    /*The current target is always the container as the event is added to it*/
    lv_obj_t * cont = lv_event_get_current_target(e);

    /*If container was clicked do nothing*/
    if(target == cont) return;

    /*Toggle the button state*/
    if(lv_obj_has_state(target, LV_STATE_USER_1)) {
        lv_obj_clear_state(target, LV_STATE_USER_1);
        lv_obj_set_style_bg_grad_color(target, lv_color_hex(0xFFFFFF), 0);
    } else {
        lv_obj_add_state(target, LV_STATE_USER_1);
        lv_obj_set_style_bg_grad_color(target, lv_color_hex(0xFF0000), 0);
    }
}

// Get functions
uint8_t get_button_state(uint8_t selectedbtn){
    if(lv_obj_get_state(buttons[selectedbtn]) & LV_STATE_USER_1){
        return 0;
    }
    else{
        return 1;
    }
}

uint8_t get_button_abc_state(char selectedbtn)
{
    switch (selectedbtn)
    {
    case 'a':
        if (lv_obj_get_state(buttons_abc[0]) & LV_STATE_USER_1)
        {
            return 0;
        }
        else
        {
            return 1;
        }
        break;
    case 'b':
        if (lv_obj_get_state(buttons_abc[1]) & LV_STATE_USER_1)
        {
            return 0;
        }
        else
        {
            return 1;
        }
        break;
    case 'c':
        if (lv_obj_get_state(buttons_abc[2]) & LV_STATE_USER_1)
        {
            return 0;
        }
        else
        {
            return 1;
        }
        break;
    default:
        return 2;
        break;
    }
}

uint8_t get_button_start(){
    if(lv_obj_get_state(button_start) & LV_STATE_USER_1){
        return 0;
    }
    else{
        return 1;
    }
}

uint8_t get_switch(uint8_t selectedswitch){
    if(lv_obj_get_state(switches[selectedswitch]) & LV_STATE_USER_1){
        return 1;
    }
    else{
        return 0;
    }
}

//Setup functions

void setup_cont_butons_abc(lv_obj_t *parent){
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_center(cont);

    lv_obj_set_style_pad_all(parent, 0, 0);

    for(int i = 0; i < 3; i++){
        buttons_abc[i] = lv_btn_create(cont);
        lv_obj_set_grid_cell(buttons_abc[i], LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
        lv_obj_set_style_bg_color(buttons_abc[i], lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_grad_color(buttons_abc[i], lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_grad_dir(buttons_abc[i], LV_GRAD_DIR_VER, 0);
        lv_obj_add_flag(buttons_abc[i], LV_OBJ_FLAG_EVENT_BUBBLE);
    }

    lv_obj_add_event_cb(cont, button_cb, LV_EVENT_CLICKED, NULL);
}

void setup_cont_buttons2(lv_obj_t *parent){
    int index = 0;
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(cont, 1, 1);
    lv_obj_center(cont);

    lv_obj_set_style_pad_all(parent, 0, 0);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            buttons[index] = lv_btn_create(cont);
            lv_obj_set_grid_cell(buttons[index], LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
            lv_obj_set_style_bg_color(buttons[index], lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_bg_grad_color(buttons[index], lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_bg_grad_dir(buttons[index], LV_GRAD_DIR_VER, 0);
            lv_obj_add_flag(buttons[index], LV_OBJ_FLAG_EVENT_BUBBLE);
            index++;
        }
    }
    lv_obj_add_event_cb(cont, button_cb, LV_EVENT_CLICKED, NULL);
}

void setup_cont_switches(lv_obj_t *parent){
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_style_bg_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_center(cont);

    lv_obj_set_style_pad_all(parent, 0, 0);

    for(int i = 0; i < 5; i++){
        switches[i] = lv_btn_create(cont);
        lv_obj_set_grid_cell(switches[i], LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
        lv_obj_set_style_bg_color(switches[i], lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_grad_color(switches[i], lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_grad_dir(switches[i], LV_GRAD_DIR_VER, 0);
        lv_obj_add_flag(switches[i], LV_OBJ_FLAG_EVENT_BUBBLE);
    }

    lv_obj_add_event_cb(cont, button_cb, LV_EVENT_CLICKED, NULL);
}

void setup_led_array(lv_obj_t *parent){
    int index = 0;
    //191/16
    double led_size = 11.6;

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_center(cont);

    for (int row = 0; row < 16; row++) {
        for (int col = 15; col >= 0; col--) {
            leds[index] = lv_obj_create(cont);
            lv_obj_set_size(leds[index], led_size, led_size);
            lv_obj_set_pos(leds[index], col * led_size, row * led_size);
            lv_obj_set_style_bg_color(leds[index], lv_color_hex(0x000000), 0);
            index++;
        }
    }
}

void setup_led_circle(lv_obj_t * parent) {
    int center_x = 525 / 2;
    int center_y = 525 / 2;
    int radius = 500 / 2;

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);

    for (int i = 0; i < 64; i++) {
        float angle = 2 * 3.14 * i / 64 - (3.14 / 2);
        int x = center_x + radius * cos(angle);
        int y = center_y + radius * sin(angle);

        leds_circle[i] = lv_obj_create(cont);
        lv_obj_set_size(leds_circle[i], 9, 9);
        lv_obj_set_pos(leds_circle[i], x - 10, y - 10);
        lv_obj_set_style_bg_color(leds_circle[i], lv_color_hex(0x000000), 0);
    }
}

void setup_ui(lv_obj_t *parent) {
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    // Create a main container with a green background
    lv_obj_t *cont_led_circle = lv_obj_create(parent);
    lv_obj_set_size(cont_led_circle, MAIN_WIDTH, MAIN_HEIGHT);
    lv_obj_align(cont_led_circle, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(cont_led_circle, lv_color_hex(0x34664B), 0);
    lv_obj_set_style_border_width(cont_led_circle, 0, 0);

    lv_obj_t *cont_main = lv_obj_create(parent);
    lv_obj_set_size(cont_main, MAIN_WIDTH - 10, MAIN_HEIGHT - 10);
    lv_obj_align(parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(cont_main, LV_OPA_TRANSP, 0);
    
    lv_obj_set_grid_dsc_array(cont_main, col_dsc, row_dsc);
    lv_obj_set_layout(cont_main, LV_LAYOUT_GRID);

    // Create containers and place them in the grid
    lv_obj_t *cont_switches = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_switches, LV_GRID_ALIGN_STRETCH, 5, 6, LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_set_style_bg_color(cont_switches, lv_color_hex(0x808080), 0);

    lv_obj_t *cont_butons_abc = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_butons_abc, LV_GRID_ALIGN_STRETCH, 1, 4, LV_GRID_ALIGN_STRETCH, 3, 2);
    lv_obj_set_style_bg_color(cont_butons_abc, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t *cont_buttons2 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_buttons2, LV_GRID_ALIGN_STRETCH, 1, 6, LV_GRID_ALIGN_STRETCH, 5, 6);
    lv_obj_set_style_bg_color(cont_buttons2, lv_color_hex(0x808080), 0);

    lv_obj_t *cont_buttons3 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_buttons3, LV_GRID_ALIGN_STRETCH, 11, 2, LV_GRID_ALIGN_STRETCH, 3, 2);
    lv_obj_set_style_bg_color(cont_buttons3, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t *cont_label = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_label, LV_GRID_ALIGN_STRETCH, 5, 6, LV_GRID_ALIGN_STRETCH, 3, 2);
    lv_obj_set_style_bg_color(cont_label, lv_color_hex(0xFFFF00), 0);

    lv_obj_t *cont_led_matrix = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_led_matrix, LV_GRID_ALIGN_STRETCH, 7, 6, LV_GRID_ALIGN_STRETCH, 5, 6);
    lv_obj_set_style_bg_color(cont_led_matrix, lv_color_hex(0xFF0000), 0);

    lv_obj_t *cont_label2 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_label2, LV_GRID_ALIGN_STRETCH, 4, 4, LV_GRID_ALIGN_STRETCH, 11, 2);
    lv_obj_set_style_bg_color(cont_label2, lv_color_hex(0xFF0000), 0);

    //Fill in the containers
    setup_cont_butons_abc(cont_butons_abc);
    setup_cont_buttons2(cont_buttons2);
    setup_cont_switches(cont_switches);
    setup_led_array(cont_led_matrix);
    setup_led_circle(cont_led_circle);

    button_start = lv_btn_create(cont_buttons3);

    lcd_display_label = lv_label_create(cont_label);
	lv_label_set_text(lcd_display_label, "Hello world\n");

    segmented_display_label = lv_label_create(cont_label2);
	lv_label_set_text(segmented_display_label, "1 2 3 4\n");

    //start button layout
    lv_obj_set_style_bg_opa(cont_buttons3, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(cont_buttons3, 1, 1);
    lv_obj_set_size(button_start, LV_PCT(100), LV_PCT(100));
    lv_obj_center(button_start);
    lv_obj_set_style_bg_color(button_start, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_grad_color(button_start, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_grad_dir(button_start, LV_GRAD_DIR_VER, 0);

    lv_obj_add_flag(button_start, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(cont_buttons3, button_cb, LV_EVENT_CLICKED, NULL);
}
#endif