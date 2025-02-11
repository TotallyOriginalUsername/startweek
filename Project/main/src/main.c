#include "statemachine.h"
#include "threads.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

// The stack size of all threads
#define STACKSIZE 1024

// Thread priority values (lower value is higher priority)
#define TMAIN_PRIORITY 8

void tmain(void) // Core thread
{
	printf("Main\n");
	startStatemachine();
}

// Define the threads
K_THREAD_DEFINE(tmain_id, STACKSIZE, tmain, NULL, NULL, NULL, TMAIN_PRIORITY, 0, 0);
