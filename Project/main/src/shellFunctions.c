#include "shellFunctions.h"
#include "sdCard.h"
#include "idle.h"

#include <stdint.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <string.h>
#include "zephyr/sys/printk.h"
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(shellCMD);

// Global tracking of locations to allow receiving locations one by one
int globalLocationsCount = 0;
struct location_new globalLocations[64] = {0};

struct trivia_question globalTrivia = {0};

static int cmd_add_loc(const struct shell *sh, size_t argc, char **argv)
{
		globalLocations[globalLocationsCount].lat = atoi(argv[1]);
		globalLocations[globalLocationsCount].lon = atoi(argv[2]);
		globalLocations[globalLocationsCount].mg_id = atoi(argv[3]);
		globalLocations[globalLocationsCount].trivia_id = atoi(argv[4]);
		printk("Adding: %lld,%lld,%d,%d\n", globalLocations[globalLocationsCount].lat,
		globalLocations[globalLocationsCount].lon, globalLocations[globalLocationsCount].mg_id,
	globalLocations[globalLocationsCount].trivia_id);

		globalLocationsCount++;
		printk("Loc count: %d\n", globalLocationsCount);
        return 0;
}

static int cmd_get_loc(const struct shell *sh, size_t argc, char **argv)
{
	struct location_new locTest[15] = {0};
	int locCount = 0;
	
	sd_get_locations(locTest, sizeof(locTest), &locCount);
	for(int i = 0; i < locCount; i++){
		shell_print(sh, "loc %d: %lld,%lld,%d,%hu\n", i,
			locTest[i].lat, locTest[i].lon, locTest[i].mg_id, locTest[i].trivia_id);
	}
	return 0;
}

static int cmd_reload_loc(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	reloadLocations();

	shell_print(sh, "done reloading\n");

	return 0;
}

static int cmd_save_loc(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	sd_set_locations(globalLocations, globalLocationsCount);

	shell_print(sh, "done saving\n");

	return 0;
}

static int cmd_start_loc(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	globalLocationsCount = 0;
	memset(globalLocations, 0, sizeof globalLocations);
	printk("Loc count: %d\n", globalLocationsCount);

	return 0;
}

static int cmd_get_progress(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int progress = sd_get_progress();
	shell_print(sh, "Progress: %d\n", progress);

	return 0;
}

static int cmd_get_score(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int score = sd_get_score();
	shell_print(sh, "Score: %d\n", score);

	return 0;
}

static int cmd_reset_progress(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	int ret = 0;

	ret = sd_clear_progress();
	if (ret < 0) {
		shell_print(sh, "Something went wrong with resetting progress\n");
	}

	return 0;
}

static int cmd_reset_score(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	int ret = 0;

	ret = sd_clear_score();
	if (ret < 0) {
		shell_print(sh, "Something went wrong with resetting score\n");
	}

	return 0;
}

static int cmd_reset_all(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	int ret = 0;

	ret = sd_setup_files();
	if (ret < 0) {
		shell_print(sh, "Something went wrong with resetting all\n");
	}

	return 0;
}

static int cmd_set_progress(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	int progress = atoi(argv[1]);

	if(progress < 0){
		printk("Please set a positive progress");
	} else {
		sd_set_progress(progress);
	}

	return 0;
}

static int cmd_set_score(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	int score = atoi(argv[1]);

	if(score < 0){
		printk("Please set a positive score");
	} else {
		sd_set_score(score);
	}

	return 0;
}

static int cmd_set_starttime(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	int starttime = atoi(argv[1]);

	sd_set_start_time(starttime);

	return 0;
}

static int cmd_set_endtime(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	int endtime = atoi(argv[1]);

	sd_set_end_time(endtime);

	return 0;
}

static int cmd_trivia_number(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	globalTrivia.questionNumber = atoi(argv[1]);

	return 0;
}

static int cmd_trivia_correct(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	globalTrivia.correct = atoi(argv[1]);

	return 0;
}

static int cmd_trivia_question(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	strncpy(globalTrivia.question, argv[1], sizeof(globalTrivia.question));
	
	return 0;
}

static int cmd_trivia_answer(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int answerNumber = atoi(argv[1]);
	switch (answerNumber) {
		case 0:
			strncpy(globalTrivia.answerA, argv[2], sizeof(globalTrivia.answerA));
			break;
		case 1:
			strncpy(globalTrivia.answerB, argv[2], sizeof(globalTrivia.answerB));
			break;
		case 2:
			strncpy(globalTrivia.answerC, argv[2], sizeof(globalTrivia.answerC));
			break;
		default:
			shell_print(sh, "Trivia only supports 3 answers\n");
			break;
	}

	return 0;
}

// Clears the shell's trivia, not on the sd
static int cmd_trivia_clear(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	memset(&globalTrivia, 0, sizeof(globalTrivia));

	return 0;
}

static int cmd_trivia_save(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	struct trivia_question testStruct = {0};
	testStruct.questionNumber = globalTrivia.questionNumber;

	sd_save_trivia_question(&globalTrivia);
	
	int ret = sd_get_trivia_question(&testStruct);
	if(ret < 0){
		shell_print(sh, "Couldn't load trivia");
	}

	return 0;
}

static int cmd_trivia_get(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int ret = 0;
	struct trivia_question testStruct = {0};
	testStruct.questionNumber = globalTrivia.questionNumber;

	for(int i = 1; i <=12; i++){
		testStruct.questionNumber = i;
		ret = sd_get_trivia_question(&testStruct);
		if(ret < 0){
			shell_print(sh, "Couldn't load trivia");
		}else {
			shell_print(sh, "Saved nr: %d,correct: %d\nQ: %s\nA: %s\nB: %s\nC: %s", testStruct.questionNumber,
			testStruct.correct, testStruct.question, testStruct.answerA, testStruct.answerB,
			testStruct.answerC);
		}
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(get_commands,
        SHELL_CMD(progress, NULL, "Get progress", cmd_get_progress),
        SHELL_CMD(score, NULL, "Get score", cmd_get_score),
        SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(reset_commands,
		SHELL_CMD(all, NULL, "Resets everything", cmd_reset_all),
        SHELL_CMD(progress, NULL, "Reset progress", cmd_reset_progress),
        SHELL_CMD(score, NULL, "Reset score", cmd_reset_score),
        SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(set_commands,
        SHELL_CMD(progress, NULL, "Set progress [value]", cmd_set_progress),
        SHELL_CMD(score, NULL, "Set score [value]", cmd_set_score),
		SHELL_CMD(starttime, NULL, "Set starttime [value]", cmd_set_starttime),
		SHELL_CMD(endtime, NULL, "Set endtime [value]", cmd_set_endtime),
        SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sd_commands,
		SHELL_CMD(get, &get_commands, "Get [type]", NULL),
        SHELL_CMD(reset, &reset_commands, "Reset [type]", NULL),
        SHELL_CMD(set, &set_commands, "Set [type]", NULL),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sd, &sd_commands, "SD commands", NULL);

// Location and Trivia commands are not under the SD command to save characters
SHELL_STATIC_SUBCMD_SET_CREATE(sub_demo,
		SHELL_CMD(add,   NULL, "Add location", cmd_add_loc),
		SHELL_CMD(get,   NULL, "Get location(s)", cmd_get_loc),
        SHELL_CMD(reload,   NULL, "Reload location", cmd_reload_loc),
		SHELL_CMD(save,   NULL, "Save location(s)", cmd_save_loc),
		SHELL_CMD(start,   NULL, "Start loc transfer", cmd_start_loc),
        SHELL_SUBCMD_SET_END
);
/* Creating root (level 0) command "demo" */
SHELL_CMD_REGISTER(loc, &sub_demo, "Demo commands", NULL);


SHELL_STATIC_SUBCMD_SET_CREATE(trivia_commands,
		SHELL_CMD(number, NULL, "Set question number", cmd_trivia_number),
		SHELL_CMD(correct, NULL, "Set correct answer", cmd_trivia_correct),
        SHELL_CMD(q, NULL, "Set question text", cmd_trivia_question),
        SHELL_CMD(a, NULL, "Set answer A/B/C(0/1/2)", cmd_trivia_answer),
		SHELL_CMD(clear, NULL, "Clear trivia transfer", cmd_trivia_clear),
        SHELL_CMD(save, NULL, "Save trivia", cmd_trivia_save),
		SHELL_CMD(get, NULL, "Get current trivia", cmd_trivia_get),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(tr, &trivia_commands, "Trivia commands", NULL);