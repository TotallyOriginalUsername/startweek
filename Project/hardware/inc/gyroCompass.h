#ifndef GYROCOMPASS_H
#define GYROCOMPASS_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

//defines
#define gyro_sample_freq 119 //see lsm6dso kconfig of zephyr for acceptable values. Note this is for accelometer and gyro
#define magno_sample_freq 155 //see lis3mdl kconfig of zephyr for acceptable values.
#define magno_sample_freq_off 0.625 //see lis3mdl kconfig of zephyr for acceptable values.

#define X_OFFSET -84.08 / 100
#define Y_OFFSET 48.01 / 100
#define Z_OFFSET 14.87 / 100

//magnetometer functions
uint8_t magnetometer_set_sampling_freq(int aFreq);
uint8_t magnetometer_init(void);
uint8_t magnetometer_exit(void);
uint8_t magnetometer_get_magneto(int16_t *aMagneto);

//gyroscope functions
uint8_t gyroscope_set_sampling_freq(int aFreq);
uint8_t gyroscope_init(void);
uint8_t gyroscope_exit(void);
uint8_t gyroscope_get_acceleration(int16_t aAcceleration[3]);
uint8_t gyroscope_get_gyro(float aGyro[3]);

//Useful data functions
uint8_t gyroscope_get_roll(int *aRoll);
uint8_t gyroscope_get_pitch(int *aPitch);
uint8_t gyroCompass_get_heading(int *aHeading);

#endif /* GYROCOMPASS_H */
