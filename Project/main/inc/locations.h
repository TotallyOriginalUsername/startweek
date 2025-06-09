#pragma once

#include <stddef.h>
#include <stdint.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/data/json.h>
#include "sdCard.h"

#define LAT_LOC_DB 51689194440	// Den Bosch: paadje langs slagboom bij de parkeerplaats onder Avans (paadje aan gebouwkant)
#define LON_LOC_DB 5286555560

struct Location {
    int64_t x;
    int64_t y;
    uint8_t mg_id; // Minigame ID, 0 if not applicable
    int x_from_sd;
    int y_from_sd;
};

int locations_load(uint16_t type, struct Location **locations, size_t *count, size_t maxLocations);