#include "sdCard.h"

#include <stdlib.h>
LOG_MODULE_REGISTER(sdCard);

#define START_TIME 0
#define END_TIME 1

#define MAX_PATH 128

/*
 *  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
 *  in ffconf.h
 */
#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_PT "/"DISK_DRIVE_NAME":"



#if defined(CONFIG_BOARD_NUCLEO_H743ZI)

static const char *type_file[] = { // File paths for different types of locations. Dirty fix to have this here.
	"/SD:/poko.txt", // pokemon locations
	"/SD:/loc.txt", // general locations
	"/SD:/trlo.txt", // trivia locations
	"/SD:/trivia.txt" // trivia questions
};

static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = {
	.type = FS_FATFS,
	.fs_data = &fat_fs,
};

static const char *disk_mount_pt = DISK_MOUNT_PT;
#endif

int sd_card_init(){
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    static const char *disk_pdrv = DISK_DRIVE_NAME;
	uint64_t memory_size_mb;
	uint32_t block_count;
	uint32_t block_size;

	if (disk_access_init(disk_pdrv) != 0) {
		LOG_ERR("Storage init ERROR!");
		return 1;
	}

	if (disk_access_ioctl(disk_pdrv,
			DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
		LOG_ERR("Unable to get sector count");
		return 1;
	}
		LOG_INF("Block count %u", block_count);

	if (disk_access_ioctl(disk_pdrv,
			DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
		LOG_ERR("Unable to get sector size");
	return 1;
	}
	LOG_INF("Sector size %u\n", block_size);

	memory_size_mb = (uint64_t)block_count * block_size;
	LOG_INF("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));

	mp.mnt_point = disk_mount_pt;

	int res = fs_mount(&mp);
    if(res == 0){
        LOG_INF("Disk mounted.\n");
    }
    else {
        LOG_ERR("Error mounting disk\n");
    	return res;
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
int sd_clear_score(){
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    int ret;
	int score = 0;
	char file_data_buffer[200];
    struct fs_file_t data_filp;

    fs_file_t_init(&data_filp);
    ret = fs_open(&data_filp, "/SD:/score.txt", FS_O_RDWR);
    if (ret) {
        LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
        return ret;
    }

	ret = fs_truncate(&data_filp, 0);
    if (ret) {
        LOG_ERR("%s -- failed to truncate file (err = %d)\n", __func__, ret);
        return ret;
    }

	sprintf(file_data_buffer, "%d\n", score);
	ret = fs_write(&data_filp, file_data_buffer, strlen(file_data_buffer));
	if (ret < 0) {
		LOG_ERR("%s -- failed to write to file (err = %d)\n", __func__, ret);
		return ret;
	} else {
		LOG_INF("%s - successfully cleared score\n", __func__);
	}

	// Close the file

	ret = fs_close(&data_filp);
	if (ret < 0) {
		LOG_ERR("%s -- failed to close file (err = %d)\n", __func__, ret);
		return ret;
	} else {
		LOG_INF("%s - successfully closed file\n", __func__);
	}

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
		LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
		return -2;
	} else {
		//LOG_ERR("%s - successfully opened file\n", __func__);
	}

	ret = fs_read(&data_filp, file_data_buffer, 200);
	fs_close(&data_filp);

	sscanf(file_data_buffer, "%d", &score);
#endif
    return score;
}

// Set the score in the file on the SD card
int sd_set_score(int score){
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
		LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
		return ret;
	} else {
		//LOG_ERR("%s - successfully opened file\n", __func__);
	}

	sprintf(file_data_buffer, "%d\n", new_score);
	ret = fs_write(&data_filp, file_data_buffer, strlen(file_data_buffer));
	fs_close(&data_filp);
#endif
	return 0;
}

// Set the progress in the file on the SD card. Honestly is just a copy of sd_get_score with a different file name.
int sd_get_progress()
{
	int progress = 0;
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int ret = 0;
	char file_data_buffer[20];
	struct fs_file_t data_filp;

	fs_file_t_init(&data_filp);

	ret = fs_open(&data_filp, "/SD:/progress.txt",  FS_O_READ);
	if (ret) {
		LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
		return ret;
	} else {
		//LOG_ERR("%s - successfully opened file\n", __func__);
	}

	ret = fs_read(&data_filp, file_data_buffer, 200);
	fs_close(&data_filp);

	sscanf(file_data_buffer, "%d", &progress);
#endif
	return progress;
}

int sd_set_progress(int progress)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int ret;
	char file_data_buffer[20];
	struct fs_file_t data_filp;

	fs_file_t_init(&data_filp);

	ret = fs_open(&data_filp, "/SD:/progress.txt", FS_O_CREATE | FS_O_WRITE);
	if (ret) {
		LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
		return ret;
	} else {
		//LOG_ERR("%s - successfully opened file\n", __func__);
	}

	sprintf(file_data_buffer, "%d\n", progress);
	ret = fs_write(&data_filp, file_data_buffer, strlen(file_data_buffer));
	if (ret)
	{
		LOG_ERR("%s -- failed to write to file (err = %d)\n", __func__, ret);
		return ret;
	}
	fs_close(&data_filp);
#endif
	return 0;
}

uint8_t sd_get_buffer(uint16_t select_file, char *buf, size_t *len, size_t max_len)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int ret = 0;
	char file_data_buffer[max_len]; // Buffer to hold file data
	struct fs_file_t data_filp;

	fs_file_t_init(&data_filp);

	if (select_file >= sizeof(type_file) / sizeof(type_file[0])) {
		LOG_ERR("No such file type known: %d", select_file);
		return -1;
	}

	ret = fs_open(&data_filp, type_file[select_file], FS_O_READ);
	if (ret) {
		LOG_ERR("Failed to open file: %s (err = %d)", type_file[select_file], ret);
		return ret;
	} else {
		LOG_MSG_DBG("Opened file: %s", type_file[select_file]);
	}

	*len = fs_read(&data_filp, file_data_buffer, sizeof(file_data_buffer) - 1);
	fs_close(&data_filp);

	if (len < 0) {
		LOG_ERR("Failed to read file: %s", type_file[select_file]);
		return -2;
	}

	if (*len >= max_len) {
		LOG_ERR("Buffer size too small. Requested: %zu, Available: %zu", *len, max_len);
		return -3;
	}

	file_data_buffer[*len] = '\0';

	memcpy(buf, file_data_buffer, *len + 1);

#endif
	return 0;
}

/**
 * @brief Get the start or end time from the SD card.
 *
 * @param type 0 for start time, 1 for end time.
 * @return int16_t >= 0 The time of the day in minutes
 * @return int16_t < 0 On error
 */
int16_t get_time(uint8_t type)
{
	int16_t time = 0;
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int ret = 0;
	char file_data_buffer[20];
	struct fs_file_t data_filp;

	fs_file_t_init(&data_filp);

	switch (type) {
		case 0: // Start time
			ret = fs_open(&data_filp, "/SD:/start.txt", FS_O_READ);
			break;
		case 1: // End time
			ret = fs_open(&data_filp, "/SD:/end.txt", FS_O_READ);
			break;
		default:
			LOG_ERR("Invalid type for get_time: %d", type);
			return -1;
	}

	if (ret) {
		LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
		return ret; // narrowing conversion from 'int' to 'int16_t' may lose data
	} else {
		//LOG_ERR("%s - successfully opened file\n", __func__);
	}

	ret = fs_read(&data_filp, file_data_buffer, 200);
	if (ret < 0) {
		LOG_ERR("%s -- failed to read file (err = %d)\n", __func__, ret);
		fs_close(&data_filp);
		return ret; // narrowing conversion from 'int' to 'int16_t' may lose data
	} else {
		// LOG_MSG_DBG("%s - successfully read file\n", __func__);
	}
	fs_close(&data_filp);

	sscanf(file_data_buffer, "%hd", &time);
#endif
	return time;
}

/**
 * @brief Get the start time from the SD card.
 *
 * @return int16_t >= 0 The start time of the day in minutes
 * @return int16_t < 0 On error
 */
int16_t sd_get_start_time()
{
	return get_time(START_TIME);
}

/**
 * @brief Get the end time from the SD card.
 *
 * @return int16_t >= 0 The end time of the day in minutes
 * @return int16_t < 0 On error
 */
int16_t sd_get_end_time()
{
	return get_time(END_TIME);
}
