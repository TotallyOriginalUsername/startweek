#include "../inc/lvgl_ui.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app, CONFIG_LOG_DEFAULT_LEVEL);

#define MAIN_WIDTH 520
#define MAIN_HEIGHT 520

lv_obj_t *buttons[15];
lv_obj_t *leds[256];

void set_led(int led, int state){
    if(state == 1){
        lv_obj_set_style_bg_color(leds[led], lv_color_hex(0xFF0000), 0);
    }
    else if(state == 0){
        lv_obj_set_style_bg_color(leds[led], lv_color_hex(0x000000), 0);
    }
    else{
        printk("set_led received an improper state");
    }
}

static void event_cb(lv_event_t * e)
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
        lv_obj_set_style_bg_color(target, lv_color_hex(0xFF0000), 0);
    } else {
        lv_obj_add_state(target, LV_STATE_USER_1);
        lv_obj_set_style_bg_color(target, lv_color_hex(0x00FF00), 0);
    }
}

uint8_t get_button_state(uint8_t selectedbtn){

    int button_state;
    if(lv_obj_get_state(buttons[selectedbtn]) & LV_STATE_USER_1){
        return 1;
    }
    else{
        return 0;
    }
}

void setup_cont_buttons1(lv_obj_t *parent){
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_t *cont = lv_obj_create(parent);
    //lv_obj_set_flex_grow(cont, 2);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    //lv_obj_align(parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_center(cont);

    lv_obj_update_layout(parent);

    lv_obj_t *btn1 = lv_btn_create(cont);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0xFF0000), 0);
    
    lv_obj_t *btn2 = lv_btn_create(cont);
    lv_obj_set_grid_cell(btn2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x00FF00), 0);
    
    lv_obj_t *btn3 = lv_btn_create(cont);
    lv_obj_set_grid_cell(btn3, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x0000FF), 0);
}

void setup_cont_buttons2(lv_obj_t *parent){
    int index = 0;
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_center(cont);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            buttons[index] = lv_btn_create(cont);
            lv_obj_set_grid_cell(buttons[index], LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
            lv_obj_set_style_bg_color(buttons[index], lv_color_hex(0xFF0000), 0);
            lv_obj_add_flag(buttons[index], LV_OBJ_FLAG_EVENT_BUBBLE);
            index++;
        }
    }
    lv_obj_add_event_cb(cont, event_cb, LV_EVENT_CLICKED, NULL);
}

void setup_led_array(lv_obj_t *parent){
    int index = 0;
    //214/16
    int led_size = 13;

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);

    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
            leds[index] = lv_obj_create(cont);
            lv_obj_set_size(leds[index], led_size, led_size);
            lv_obj_set_pos(leds[index], col * led_size, row * led_size);
            lv_obj_set_style_bg_color(leds[index], lv_color_hex(0x000000), 0);
            index++;
        }
    }
}

void setup_ui(lv_obj_t *parent) {
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
    LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    // Create a main container with a green background
    lv_obj_t *cont_main = lv_obj_create(parent);
    lv_obj_set_size(cont_main, MAIN_WIDTH, MAIN_HEIGHT);
    lv_obj_align(parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(cont_main, lv_color_hex(0x00FF00), 0);
    
    lv_obj_set_grid_dsc_array(cont_main, col_dsc, row_dsc);
    lv_obj_set_layout(cont_main, LV_LAYOUT_GRID);

    // Create containers and place them in the grid
    lv_obj_t *cont_switches = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_switches, LV_GRID_ALIGN_STRETCH, 5, 6, LV_GRID_ALIGN_STRETCH, 0, 2);
    lv_obj_set_style_bg_color(cont_switches, lv_color_hex(0x808080), 0);

    lv_obj_t *cont_buttons1 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_buttons1, LV_GRID_ALIGN_STRETCH, 1, 4, LV_GRID_ALIGN_STRETCH, 2, 2);
    lv_obj_set_style_bg_color(cont_buttons1, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t *cont_buttons2 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_buttons2, LV_GRID_ALIGN_STRETCH, 0, 6, LV_GRID_ALIGN_STRETCH, 4, 6);
    lv_obj_set_style_bg_color(cont_buttons2, lv_color_hex(0x808080), 0);

    lv_obj_t *cont_buttons3 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_buttons3, LV_GRID_ALIGN_STRETCH, 11, 2, LV_GRID_ALIGN_STRETCH, 2, 2);
    lv_obj_set_style_bg_color(cont_buttons3, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t *cont_label = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_label, LV_GRID_ALIGN_STRETCH, 5, 6, LV_GRID_ALIGN_STRETCH, 2, 2);
    lv_obj_set_style_bg_color(cont_label, lv_color_hex(0xFFFF00), 0);

    lv_obj_t *cont_led_matrix = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_led_matrix, LV_GRID_ALIGN_STRETCH, 7, 6, LV_GRID_ALIGN_STRETCH, 4, 6);
    lv_obj_set_style_bg_color(cont_led_matrix, lv_color_hex(0xFF0000), 0);

    lv_obj_t *cont_label2 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_label2, LV_GRID_ALIGN_STRETCH, 4, 4, LV_GRID_ALIGN_STRETCH, 10, 2);
    lv_obj_set_style_bg_color(cont_label2, lv_color_hex(0xFF0000), 0);

    //Fill in the containers
    setup_cont_buttons1(cont_buttons1);
    setup_cont_buttons2(cont_buttons2);
    setup_led_array(cont_led_matrix);
    lv_obj_t *btn3 = lv_btn_create(cont_buttons3);

    lv_obj_t *label = lv_label_create(cont_label);
	lv_label_set_text(label, "Hello world\n");

    lv_obj_t *label2 = lv_label_create(cont_label2);
	lv_label_set_text(label2, "1 2 3 4\n");
}
