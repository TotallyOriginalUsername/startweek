#include "sdCard.h"
#include <zephyr/data/json.h>


#include <stdlib.h>
LOG_MODULE_REGISTER(sdCard);

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
	"/SD:/trivia.txt" // trivia locations
};

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
		LOG_ERR("%s -- failed to open file (err = %d)\n", __func__, ret);
		return -2;
	} else {
		//LOG_ERR("%s - successfully opened file\n", __func__);
	}

	sprintf(file_data_buffer, "%d\n", new_score);
	ret = fs_write(&data_filp, file_data_buffer, strlen(file_data_buffer));
	fs_close(&data_filp);
#endif
	return 0;
}



#define MAX_LEN 32// ugly as fuq
#define MAX_QUIZZES 4// f me
/*/
struct Quiz {
	char question[MAX_LEN];
	char answerA[MAX_LEN];
	char answerB[MAX_LEN];
	char answerC[MAX_LEN];
	int correct;
};

struct QuizSet {
	struct Quiz quizzes[MAX_QUIZZES]; // ugly as fuq
};

static const struct json_obj_descr quiz_descr[] = {
	JSON_OBJ_DESCR_PRIM(struct Quiz, question, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct Quiz, answerA, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct Quiz, answerB, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct Quiz, answerC, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct Quiz, correct, JSON_TOK_NUMBER),
};


static const struct json_obj_descr quizset_descr[] = {
	JSON_OBJ_DESCR_ARRAY(struct QuizSet, quizzes, MAX_QUIZZES, quiz_descr, ARRAY_SIZE(quiz_descr)),
};*/
/*
struct Quiz sd_get_trivia(int trivia_index){
	struct QuizSet questions;
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
    int ret = 0;
    char file_data_buffer[50];
    struct fs_file_t data_filp;

    fs_file_t_init(&data_filp);

    ret = fs_open(&data_filp, "/SD:/trivia.txt", FS_O_READ);
	if (ret) {
		printk("%s -- failed to open file (err = %d)\n", __func__, ret);
		return -2;
	} else {
		//printk("%s - successfully opened file\n", __func__);
	}

	ret = fs_read(&data_filp, file_data_buffer, 200);
	fs_close(&data_filp);

	int ret = json_obj_parse(json_blob, strlen(json_blob), quizset_descr, ARRAY_SIZE(quizset_descr), &quiz_set);

	//sscanf(file_data_buffer, "%s", &question); // this wont work

#else
	strcpy(questions.quizzes[0].question , "test question");
	strcpy(questions.quizzes[0].answerA , "a = wrong");
	strcpy(questions.quizzes[0].answerB , "b = wright");
	strcpy(questions.quizzes[0].answerC , "c = wrong2");
	questions.quizzes[0].correct = 0;
#endif
    return questions.quizzes[trivia_index];
}
*/



uint8_t sd_get_trivia(uint16_t type, char *buf, size_t *len, size_t max_len)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int ret = 0;
	char file_data_buffer[max_len]; // Buffer to hold file data
	struct fs_file_t data_filp;

	fs_file_t_init(&data_filp);

	if (type >= sizeof(type_file) / sizeof(type_file[0])) {
		LOG_ERR("No such file type known: %d", type);
		return -1;
	}

	ret = fs_open(&data_filp, type_file[type], FS_O_READ);
	if (ret) {
		LOG_ERR("Failed to open file: %s (err = %d)", type_file[type], ret);
		return -2;
	} else {
		LOG_MSG_DBG("Opened file: %s", type_file[type]);
	}

	*len = fs_read(&data_filp, file_data_buffer, sizeof(file_data_buffer) - 1);
	fs_close(&data_filp);

	if (len < 0) {
		LOG_ERR("Failed to read file: %s", type_file[type]);
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





uint8_t sd_get_locations(uint16_t type, char *buf, size_t *len, size_t max_len)
{
#if defined(CONFIG_BOARD_NUCLEO_H743ZI)
	int ret = 0;
	char file_data_buffer[max_len]; // Buffer to hold file data
	struct fs_file_t data_filp;

	fs_file_t_init(&data_filp);

	if (type >= sizeof(type_file) / sizeof(type_file[0])) {
		LOG_ERR("No such file type known: %d", type);
		return -1;
	}

	ret = fs_open(&data_filp, type_file[type], FS_O_READ);
	if (ret) {
		LOG_ERR("Failed to open file: %s (err = %d)", type_file[type], ret);
		return -2;
	} else {
		LOG_MSG_DBG("Opened file: %s", type_file[type]);
	}

	*len = fs_read(&data_filp, file_data_buffer, sizeof(file_data_buffer) - 1);
	fs_close(&data_filp);

	if (len < 0) {
		LOG_ERR("Failed to read file: %s", type_file[type]);
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