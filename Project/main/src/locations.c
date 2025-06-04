//
// Created by flits on 6/4/25.
//
#include "locations.h"

#include <stdlib.h>
#include <zephyr/data/json.h>
#include "sdCard.h"

#define MAX_TYPES (1 << (sizeof(uint16_t) * 8))
#define BUFFER_SIZE 512 // dirty to have this here.

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
    char json_buf[BUFFER_SIZE];
    size_t len = 0;
    int ret = st_get_locations(type, json_buf, &len, BUFFER_SIZE);
    if (ret != 0)
        return ret;

    struct Location *locArray = malloc(sizeof(struct Location) * maxLocations);
    if (!locArray)
        return -2;

    struct json_obj_descr loc_descr[] = {
        JSON_OBJ_DESCR_PRIM(struct Location, x, JSON_TOK_NUMBER),
        JSON_OBJ_DESCR_PRIM(struct Location, y, JSON_TOK_NUMBER),
    };

    struct json_obj json_arr;
    ret = json_arr_separate_object_parse_init(&json_arr, json_buf, len);
    if (ret < 0) {
        free(locArray);
        return -3;
    }

    size_t i = 0;
    for (; i < maxLocations; ++i) {
        memset(&locArray[i], 0, sizeof(struct Location));
        ret = json_arr_separate_parse_object(&json_arr, loc_descr, 2, &locArray[i]);
        if (ret == 0) break; // End of array
        if (ret < 0) {
            // Log error and clean up

            free(locArray);
            return -3;
        }
    }

    *count = i;
    *locations = locArray;
    return 0;
}

