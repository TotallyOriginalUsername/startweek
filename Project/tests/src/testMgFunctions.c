#ifdef CONFIG_TEST_MINIGAME_FUNCTIONS

#include <zephyr/ztest.h>
#include "minigame1.h"
#include "minigame2.h"
#include "minigame3.h"
#include "minigame4.h"
#include "minigame5.h"
#include "minigame6.h"
#include "minigame7.h"
#include "minigame8.h"
#include "minigame9.h"
#include "minigame10.h"

ZTEST_SUITE(testMg, NULL, NULL, NULL, NULL, NULL);

//Please keep the tests ordered from mg1 to mg10

// Minigame 7's functions
ZTEST(testMg, generate_fruit_mg7)
{
	uint16_t fruit = 0b0000000100000000;
    uint16_t new_fruit = generate_fruit_mg7(fruit);

	bool generated_left  = new_fruit == (fruit << 1);
	bool generated_right = new_fruit == (fruit >> 1);

    zassert_true(generated_left || generated_right);
}

ZTEST(testMg, update_fruit_mg7)
{
	uint16_t fruit_masks[16] = {0};
    fruit_masks[0] = 0b1000000000000000;
    update_fruit_mg7(fruit_masks);
    zassert_equal(fruit_masks[1], 0b1000000000000000);
}

ZTEST(testMg, hit_detection_mg7)
{
    uint16_t fruit_masks[16] = {0};
    uint16_t plate_mask = 0b1110000000000000;
	bool hit = hit_detection_mg7(plate_mask, fruit_masks);

    zassert_equal(hit, 0);

    fruit_masks[15] = 0b0000000000010000;
	hit = hit_detection_mg7(plate_mask, fruit_masks);
    zassert_equal(hit, 1);

    fruit_masks[15] = 0b0100000000000000;
	hit = hit_detection_mg7(plate_mask, fruit_masks);
    zassert_equal(hit, 0);
}

// Minigame 8's functions
ZTEST(testMg, get_any_btnmatrix)
{
	int input = get_any_btnmatrix();
	zassert_within(input,0, 15);
}

ZTEST(testMg, calculate_score_mg8)
{
	uint32_t remaining_time = 0;
	uint32_t score = 0;

	score = calculate_score_mg8(remaining_time);
	zassert_equal(score,0);

	remaining_time = 10000;
	score = 1000;
	score = calculate_score_mg8(remaining_time);
	zassert_equal(score,1000);

	remaining_time = 5000;
	score = 500;
	score = calculate_score_mg8(remaining_time);
	zassert_equal(score,500);
}

// Minigame 9's functions
ZTEST(testMg, get_input_mg9)
{
	int input = get_input_mg9();
	zassert_within(input,0, 2);
}

ZTEST(testMg, check_input_mg9)
{
	uint8_t generated_number = 0;
	uint8_t next_generated_number = 10;
	uint8_t user_input = 1;
	bool result = 0;

	result = check_input_mg9(generated_number, next_generated_number, user_input);
	zassert_equal(result,0);

	generated_number = 20;
	result = check_input_mg9(generated_number, next_generated_number, user_input);
	zassert_equal(result,1);

	next_generated_number = 40;
	user_input = 2;
	result = check_input_mg9(generated_number, next_generated_number, user_input);
	zassert_equal(result,1);
}

// Minigame 10's functions
ZTEST(testMg, generate_mole)
{
	int mole_position = generate_mole();
	zassert_within(mole_position,0, 15);
}

ZTEST(testMg, check_input_mg10)
{
	uint8_t btnmatrix_all_pressed[16] = {0};
	uint8_t btnmatrix_unpressed[16] =  {[0 ... 15] = 1 };
	uint8_t mole_position = 0;
	bool mole_caught = 0;

	mole_caught = check_input_mg10(btnmatrix_all_pressed, mole_position);
	zassert_equal(mole_caught, 0);
	mole_caught = check_input_mg10(btnmatrix_unpressed, mole_position);
	zassert_equal(mole_caught, 0);

	btnmatrix_unpressed[0] = 0;
	mole_position = 0;
	mole_caught = check_input_mg10(btnmatrix_unpressed, mole_position);
	zassert_equal(mole_caught, 1);
}

#endif