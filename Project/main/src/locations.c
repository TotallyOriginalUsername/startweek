//
// Created by Kasper Janssen on 04/06/25.
//
#include "locations.h"

#include <lcd.h>
#include <stdlib.h>
#include <zephyr/data/json.h>
#include "sdCard.h"

LOG_MODULE_REGISTER(locations);

#define MAX_TYPES (1 << (sizeof(uint16_t) * 8))
#define BUFFER_SIZE 8192 // dirty to have this here.

/**
 * @brief Load locations from a file based on the type.
 *
 * @param type The type of locations to load.
 * @param locations Pointer to an array of Location pointers to be filled.
 * @param count Pointer to a size_t variable to store the number of loaded locations.
 * @param maxLocations Maximum number of locations to load.
 * @return 0 on success, negative error code on failure.
 * @return 1 if locations of this type are already loaded.
 * @note Possible leak if locations are already loaded and this function is called again.
 */
int locations_load(uint16_t type, struct Location **locations, size_t *count, size_t maxLocations)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    char json_buf[BUFFER_SIZE];
    size_t len = 0;
    int ret = sd_get_locations(type, json_buf, &len, BUFFER_SIZE);
    if (ret != 0)
        return ret;

    struct Location *locArray = malloc(sizeof(struct Location) * maxLocations);
    if (!locArray)
        return -2;

    struct json_obj_descr loc_descr[] = {
        JSON_OBJ_DESCR_PRIM_NAMED(struct Location, "x", x_int, JSON_TOK_NUMBER),
        JSON_OBJ_DESCR_PRIM_NAMED(struct Location, "y", y_int, JSON_TOK_NUMBER),
    };

    struct json_obj json_arr;
    ret = json_arr_separate_object_parse_init(&json_arr, json_buf, len);
    if (ret < 0) {
        LOG_ERR("Parse init error: %d", ret);
        free(locArray);
        return ret;
    }

    size_t i = 0;
    while (i < maxLocations) {
        memset(&locArray[i], 0, sizeof(struct Location));
        ret = json_arr_separate_parse_object(&json_arr, loc_descr, 2, &locArray[i]);
        if (ret == 0)
        {
            LOG_INF("End of array reached at index %zu", i);
            lcdStringWrite("End of array reached");
            k_msleep(2000); // Wait to display the message
            break; // End of array
        }
        if (ret < 0) {
            LOG_ERR("Parse error at index %zu: %d", i, ret);
            // Free allocated locations before returning
            free(locArray);
            return ret;
        }
        i++;
        locArray[i].x = (int64_t)locArray[i].x_int * 1000; // convert back to nanodegrees
        locArray[i].y = (int64_t)locArray[i].y_int * 1000;
    }

    *count = i;
    *locations = locArray;
#endif
    return 0;
}
