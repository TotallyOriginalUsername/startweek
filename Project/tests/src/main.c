/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include "threads.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST(framework_tests, test_assert)
{
	zassert_true(1, "1 was false");
	zassert_false(0, "0 was true");
	zassert_is_null(NULL, "NULL was not NULL");
	zassert_not_null("foo", "\"foo\" was NULL");
	zassert_equal(1, 1, "1 was not equal to 1");
	zassert_equal_ptr(NULL, NULL, "NULL was not equal to NULL");
}

#if defined(CONFIG_ARCH_POSIX)
#define STACKSIZE2 16000
#define TLVGL_PRIORITY 3

void lvgl_task_handler_loop() {
    while (1) {
		//k_cpu_idle();
        lv_task_handler();
		//for native_sim
        k_sleep(K_MSEC(1));
    }
}
#endif

void test_main(void)
{
#if defined(CONFIG_ARCH_POSIX)
	const struct device *display_dev;
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		printk("Device not ready, aborting test");
	}

	display_blanking_off(display_dev);

	setup_ui(lv_scr_act());
#endif
	disableAllThreads();

	ztest_run_all(NULL, false, 1, 1);

	ztest_verify_all_test_suites_ran();
}

#if defined(CONFIG_ARCH_POSIX)
K_THREAD_DEFINE(tlvgl_id, STACKSIZE2, lvgl_task_handler_loop, NULL, NULL, NULL, TLVGL_PRIORITY, 0, 0);
#endif