#ifdef CONFIG_TEST_HELPER_FUNCTIONS

#include <zephyr/ztest.h>
#include "helperFunctions.h"

ZTEST_SUITE(testHelperFunctions, NULL, NULL, NULL, NULL, NULL);

ZTEST(testHelperFunctions, test_btnmatrix_in_to_out)
{
    uint8_t input[16] = {0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0};
    uint8_t expected_output[4] = {0b00001000, 0b00000100, 0b00000010, 0b00000001};
    uint8_t output[4] = {0};
	btnmatrix_in_to_out(input, output);

	for (int i = 0; i < 4; i++) {
        zassert_equal(output[i], expected_output[i]);
    }
}

ZTEST(testHelperFunctions, btnmatrix_to_ledmatrix)
{
    uint8_t input[4] = {0b00001000, 0b00000100, 0b00000010, 0b00000001};
    uint16_t expected_output[16] = {0b1111000000000000, 0b1111000000000000, 0b1111000000000000,
    0b1111000000000000, 0b0000111100000000, 0b0000111100000000, 0b0000111100000000,
    0b0000111100000000, 0b0000000011110000, 0b0000000011110000, 0b0000000011110000,
    0b0000000011110000, 0b0000000000001111, 0b0000000000001111, 0b0000000000001111,
    0b0000000000001111};
    uint16_t output[16] = {0};

	btnmatrix_to_ledmatrix(input, output);

	for (int i = 0; i < 16; i++) {
        zassert_equal(output[i], expected_output[i]);
    }
}

ZTEST(testHelperFunctions, led_matrix_hit_detection)
{
    uint8_t row = 15;
    uint16_t fruit_masks[16] = {0};
    uint16_t plate_mask = 0b1110000000000000;
	bool hit = led_matrix_hit_detection(fruit_masks, plate_mask, row);

    zassert_equal(hit, 0);

    fruit_masks[15] = 0b0000000000010000;
	hit = led_matrix_hit_detection(fruit_masks, plate_mask, row);
    zassert_equal(hit, 1);

    fruit_masks[15] = 0b0100000000000000;
	hit = led_matrix_hit_detection(fruit_masks, plate_mask, row);
    zassert_equal(hit, 0);
}

ZTEST(testHelperFunctions, led_matrix_scroll_down)
{
	uint16_t fruit_masks[16] = {0};
    fruit_masks[0] = 0b1000000000000000;
    led_matrix_scroll_down(fruit_masks);
    zassert_equal(fruit_masks[1], 0b1000000000000000);
}
#endif