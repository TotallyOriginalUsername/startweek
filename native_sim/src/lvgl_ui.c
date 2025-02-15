#include "../inc/lvgl_ui.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app, CONFIG_LOG_DEFAULT_LEVEL);

void setup_ui(lv_obj_t * parent) {
    // Create a main container with a green background
    lv_obj_t * cont_main = lv_obj_create(parent);
    lv_obj_set_size(cont_main, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_style_bg_color(cont_main, lv_color_hex(0x00FF00), 0);

    int32_t width = lv_obj_get_width(cont_main);
    int32_t height = lv_obj_get_height(cont_main);
    printk("Width: %d, Height: %d\n", width, height);

    // Create containers for all groups
    lv_obj_t * cont_switches = lv_obj_create(cont_main);
    lv_obj_set_size(cont_switches, 6 * LV_HOR_RES / 13, LV_VER_RES / 13);
    lv_obj_set_pos(cont_switches, 5 * LV_HOR_RES / 13, 1 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_switches, lv_color_hex(0x808080), 0);

    lv_obj_t * cont_buttons1 = lv_obj_create(cont_main);
    lv_obj_set_size(cont_buttons1, 4 * LV_HOR_RES / 13, LV_VER_RES / 13);
    lv_obj_set_pos(cont_buttons1, 2 * LV_HOR_RES / 13, 2 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_buttons1, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t * cont_buttons2 = lv_obj_create(cont_main);
    lv_obj_set_size(cont_buttons2, 4 * LV_HOR_RES / 13, 4 * LV_VER_RES / 13);
    lv_obj_set_pos(cont_buttons2, 1 * LV_HOR_RES / 13, 3 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_buttons2, lv_color_hex(0x808080), 0);

    lv_obj_t * cont_buttons3 = lv_obj_create(cont_main);
    lv_obj_set_size(cont_buttons3, LV_HOR_RES / 13, LV_VER_RES / 13);
    lv_obj_set_pos(cont_buttons3, 12 * LV_HOR_RES / 13, 2 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_buttons3, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t * cont_label = lv_obj_create(cont_main);
    lv_obj_set_size(cont_label, 6 * LV_HOR_RES / 13, LV_VER_RES / 13);
    lv_obj_set_pos(cont_label, 6 * LV_HOR_RES / 13, 2 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_label, lv_color_hex(0xFFFF00), 0);

    lv_obj_t * cont_array = lv_obj_create(cont_main);
    lv_obj_set_size(cont_array, 4 * LV_HOR_RES / 13, 4 * LV_VER_RES / 13);
    lv_obj_set_pos(cont_array, 10 * LV_HOR_RES / 13, 3 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_array, lv_color_hex(0xFF0000), 0);

    lv_obj_t * cont_label2 = lv_obj_create(cont_main);
    lv_obj_set_size(cont_label2, 4 * LV_HOR_RES / 13, LV_VER_RES / 13);
    lv_obj_set_pos(cont_label2, 4 * LV_HOR_RES / 13, 8 * LV_VER_RES / 13);
    lv_obj_set_style_bg_color(cont_label2, lv_color_hex(0xFF0000), 0);
}