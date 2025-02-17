#include "../inc/lvgl_ui.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app, CONFIG_LOG_DEFAULT_LEVEL);

#define MAIN_WIDTH 520
#define MAIN_HEIGHT 520

lv_obj_t *buttons[15];

uint8_t get_button_state(uint8_t selectedbtn){
    int button_state;
    if(lv_obj_get_state(buttons[selectedbtn]) == LV_EVENT_PRESSING){
        return 0;
    }
    else{
        return 1;
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
            //lv_obj_add_state(buttons[index], LV_STATE_PRESSED);
            lv_obj_clear_state(buttons[index], LV_STATE_PRESSED);
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

    lv_obj_t *cont_array = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_array, LV_GRID_ALIGN_STRETCH, 7, 6, LV_GRID_ALIGN_STRETCH, 4, 6);
    lv_obj_set_style_bg_color(cont_array, lv_color_hex(0xFF0000), 0);

    lv_obj_t *cont_label2 = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_label2, LV_GRID_ALIGN_STRETCH, 4, 4, LV_GRID_ALIGN_STRETCH, 10, 2);
    lv_obj_set_style_bg_color(cont_label2, lv_color_hex(0xFF0000), 0);

    //Fill in the containers
    setup_cont_buttons1(cont_buttons1);
    setup_cont_buttons2(cont_buttons2);
    lv_obj_t *btn3 = lv_btn_create(cont_buttons3);

    lv_obj_t *label = lv_label_create(cont_label);
	lv_label_set_text(label, "Hello world\n");

    lv_obj_t *label2 = lv_label_create(cont_label2);
	lv_label_set_text(label2, "1 2 3 4\n");
}
