#ifndef SDCARD_H
#define SDCARD_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>
#include <zephyr/sys/printk.h>
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
#include <ff.h>
#endif
#include <string.h>
#include <zephyr/data/json.h>

#define MAX_POKEMON 32

struct PokemonLocation {
    uint64_t x;
    uint64_t y;
};

struct PokemonLocationsWrapper {
    struct PokemonLocation locations[MAX_POKEMON];
    size_t locations_count;
};

static const struct json_obj_descr pokemon_location_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct PokemonLocation, x, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct PokemonLocation, y, JSON_TOK_NUMBER),
};

static const struct json_obj_descr pokemon_locations_wrapper_descr[] = {
    JSON_OBJ_DESCR_OBJ_ARRAY(struct PokemonLocationsWrapper, locations, MAX_POKEMON, locations_count, pokemon_location_descr, ARRAY_SIZE(pokemon_location_descr)),
};

uint8_t sd_card_init();
void sd_card_unmount();

uint8_t sd_clear_score();
int sd_get_score();
uint8_t sd_set_score(int score);

int64_t sd_get_pokemon_locations(struct PokemonLocation** locations, size_t* pokemon_count);

#endif // SDCARD_H