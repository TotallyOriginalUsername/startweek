#include "sdCard.h"

#include <lcd.h>
#include <stdlib.h>

#define MAX_PATH 128

/*
 *  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
 *  in ffconf.h
 */
#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_PT "/"DISK_DRIVE_NAME":"

#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = {
	.type = FS_FATFS,
	.fs_data = &fat_fs,
};

static const char *disk_mount_pt = DISK_MOUNT_PT;
#endif

uint8_t sd_card_init(){
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    static const char *disk_pdrv = DISK_DRIVE_NAME;
	uint64_t memory_size_mb;
	uint32_t block_count;
	uint32_t block_size;

	if (disk_access_init(disk_pdrv) != 0) {
		printk("Storage init ERROR!");
		return 1;
	}

	if (disk_access_ioctl(disk_pdrv,
			DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
		printk("Unable to get sector count");
		return 1;
	}
		printk("Block count %u", block_count);

	if (disk_access_ioctl(disk_pdrv,
			DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
		printk("Unable to get sector size");
	return 1;
	}
	printk("Sector size %u\n", block_size);

	memory_size_mb = (uint64_t)block_count * block_size;
	printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));

	mp.mnt_point = disk_mount_pt;

	int res = fs_mount(&mp);
    if(res == 0){
        printk("Disk mounted.\n");
    }
    else {
        printk("Error mounting disk\n");
    }
#endif
    return 0;
}

void sd_card_unmount(){
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    fs_unmount(&mp);
#endif
}

// Clear the score from the SD card
uint8_t sd_clear_score(){
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    int ret;
	int score = 0;
	char file_data_buffer[200];
    struct fs_file_t data_filp;

    fs_file_t_init(&data_filp);
    ret = fs_open(&data_filp, "/SD:/score.txt", FS_O_RDWR);
    if (ret) {
        printk("%s -- failed to open file (err = %d)\n", __func__, ret);
        return -2;
    }

	ret = fs_truncate(&data_filp, 0);
    if (ret) {
        printk("%s -- failed to truncate file (err = %d)\n", __func__, ret);
        return -2;
    }

	sprintf(file_data_buffer, "%d\n", score);
	ret = fs_write(&data_filp, file_data_buffer, strlen(file_data_buffer));

	fs_close(&data_filp);
#endif
    return 0;
}

// Get the score from the SD card
int sd_get_score(){
	int score = 0;
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    int ret = 0;
    char file_data_buffer[20];
    struct fs_file_t data_filp;

    fs_file_t_init(&data_filp);

    ret = fs_open(&data_filp, "/SD:/score.txt", FS_O_READ);
	if (ret) {
		printk("%s -- failed to open file (err = %d)\n", __func__, ret);
		return -2;
	} else {
		//printk("%s - successfully opened file\n", __func__);
	}

	ret = fs_read(&data_filp, file_data_buffer, 200);
	fs_close(&data_filp);

	sscanf(file_data_buffer, "%d", &score);
#endif
    return score;
}

// Set the score in the file on the SD card
uint8_t sd_set_score(int score){
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    int ret;
	int current_score;
	int new_score;
    char file_data_buffer[20];
    struct fs_file_t data_filp;

	current_score = sd_get_score();
	new_score = current_score+score;

    fs_file_t_init(&data_filp);

    ret = fs_open(&data_filp, "/SD:/score.txt", FS_O_WRITE);
	if (ret) {
		printk("%s -- failed to open file (err = %d)\n", __func__, ret);
		return -2;
	} else {
		//printk("%s - successfully opened file\n", __func__);
	}

	sprintf(file_data_buffer, "%d\n", new_score);
	ret = fs_write(&data_filp, file_data_buffer, strlen(file_data_buffer));
	fs_close(&data_filp);
#endif
	return 0;
}

int64_t sd_get_pokemon_locations(struct PokemonLocation **locations, size_t *pokemon_count)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	static struct PokemonLocation *static_locations = NULL;
	int ret;
	char file_data_buffer[256]; // Buffer to hold file data
	struct fs_file_t data_filp;

	// Free previously allocated locations if they exist
	if (static_locations) {
		free(static_locations);
		static_locations = NULL;
	}

	fs_file_t_init(&data_filp);

	ret = fs_open(&data_filp, "/SD:/pokemon_locations.txt", FS_O_READ);
	if (ret) {
		printk("%s -- failed to open file (err = %d)\n", __func__, ret);

		// test
		char buf[32];
		sprintf(buf, "Failed to open file: %d", ret);
		lcdStringWrite(buf);
		k_msleep(1000);

		return -2;
	} else {
		//printk("%s - successfully opened file\n", __func__);
	}

	ssize_t len = fs_read(&data_filp, file_data_buffer, sizeof(file_data_buffer) - 1);
	fs_close(&data_filp);
	if (len < 0) {
		printk("Failed to read file\n");

		// test
		lcdStringWrite("Failed to read file");
		k_msleep(1000);

		return -2;
	}
	file_data_buffer[len] = '\0';

	struct PokemonLocationsWrapper wrapper = {0};
	ret = json_obj_parse(file_data_buffer, len, pokemon_locations_wrapper_descr, ARRAY_SIZE(pokemon_locations_wrapper_descr), &wrapper);
	if (ret < 0) {
		printk("JSON parse error: %d\n", ret);

		// test
		lcdStringWrite("JSON parse error");
		k_msleep(1000);

		return -2;
	}

	*pokemon_count = wrapper.locations_count;
	static_locations = malloc(wrapper.locations_count * sizeof(struct PokemonLocation));
	if (static_locations == NULL) {
		printk("Memory allocation failed\n");

		// test
		lcdStringWrite("Memory allocation failed");
		k_msleep(1000);

		return -2;
	}
	memcpy(static_locations, wrapper.locations, wrapper.locations_count * sizeof(struct PokemonLocation));
	*locations = static_locations;

#endif
	return 0;
}
