#include "buzzers.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

static struct pwm_dt_spec buzzer1 = PWM_DT_SPEC_GET(DT_ALIAS(buzzer1));
static struct pwm_dt_spec buzzer2 = PWM_DT_SPEC_GET(DT_ALIAS(buzzer2));
static struct pwm_dt_spec buzzer3 = PWM_DT_SPEC_GET(DT_ALIAS(buzzer3));

static bool isInit = false;

/** Specify PWM period in seconds */
#define PWM_SEC(x) (PWM_MSEC(x) * 1000UL)
/** Specify PWM frequency in hertz */
#define PWM_HZ(x) (PWM_SEC(1UL) / (x))

/**
 * @brief Initializes the buzzers.
 *
 * This function checks if the PWM devices associated with the buzzers are ready.
 *
 * @return uint8_t 0 on success, 1 if any PWM device is not ready.
 */
uint8_t buzzersInit()
{
    if (!pwm_is_ready_dt(&buzzer1))
    {
        printk("Error: PWM device %s is not ready\n", buzzer1.dev->name);
        return 1;
    }
    if (!pwm_is_ready_dt(&buzzer2))
    {
        printk("Error: PWM device %s is not ready\n", buzzer2.dev->name);
        return 1;
    }
    if (!pwm_is_ready_dt(&buzzer3))
    {
        printk("Error: PWM device %s is not ready\n", buzzer3.dev->name);
        return 1;
    }

    isInit = true;

    return 0;
}

/**
 * @brief Sets the PWM frequency for a specific buzzer.
 *
 * This function sets the PWM frequency for the specified buzzer.
 * The accepted frequency range is 20Hz to 20kHz.
 *
 * @param[in] aBuzzerNum The number of the buzzer (1, 2, or 3).
 * @param[in] aFreq The frequency to set (between 20 and 20000 Hz).
 *
 * @return uint8_t 0 on success, 1 if buzzers are not initialized, 2 if the buzzer number is incorrect, 3 if the frequency is out of range.
 */
uint8_t buzzerSetPwm(int aBuzzerNum, int aFreq)
{
    uint32_t period = 0;

    if (!isInit)
    {
        printf("Buzzers not initialized");
        return 1;
    }

    if (aBuzzerNum < 1 || aBuzzerNum > 3)
    {
        printf("Incorrect buzzer number\n");
        return 2;
    }
    if (aFreq < 20 || aFreq > 20000)
    {
        printf("Frequency outside accepted range\n");
        return 3;
    }
    period = PWM_HZ(aFreq);

    switch (aBuzzerNum)
    {
    case 1:
        pwm_set_dt(&buzzer1, period, period * 0.5);
        break;

    case 2:
        pwm_set_dt(&buzzer2, period, period * 0.5);
        break;

    case 3:
        pwm_set_dt(&buzzer3, period, period * 0.5);
        break;
    }

    return 0;
}

/**
 * @brief Turns off a specific buzzer.
 *
 * This function turns off the specified buzzer by setting its PWM period to 0.
 *
 * @param[in] aBuzzerNum The number of the buzzer (1, 2, or 3).
 *
 * @return uint8_t 0 on success, 1 if buzzers are not initialized, 2 if the buzzer number is incorrect.
 */
uint8_t buzzerTurnOff(int aBuzzerNum)
{
    uint32_t period = 0;

    if (!isInit)
    {
        printf("Buzzers not initialized");
        return 1;
    }

    if (aBuzzerNum < 1 || aBuzzerNum > 3)
    {
        printf("Incorrect buzzer number\n");
        return 2;
    }

    switch (aBuzzerNum)
    {
    case 1:
        pwm_set_dt(&buzzer1, period, period * 0.5);
        break;

    case 2:
        pwm_set_dt(&buzzer2, period, period * 0.5);
        break;

    case 3:
        pwm_set_dt(&buzzer3, period, period * 0.5);
        break;
    }
    return 0;
}

/**
 * @brief Turns off all buzzers.
 *
 * This function turns off all buzzers by calling the buzzerTurnOff function for each buzzer.
 *
 * @return uint8_t Always returns 0.
 */
uint8_t buzzersExit()
{
    buzzerTurnOff(1);
    buzzerTurnOff(2);
    buzzerTurnOff(3);

	return 0;
}
