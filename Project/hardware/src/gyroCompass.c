#include "gyroCompass.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <zephyr/kernel.h>

#define M_PIL 3.141592653589793238462643383279502884L
#define zero 0
#define MINDELTATRIG 1

// rotation matrix for magnetometer
const int16_t rotation_magno[3][3] = {
	{1, 0, 0},
	{0, -1, 0},
	{0, 0, -1}};

// rotation matrix for accelometer
const int16_t rotation_accel[3][3] = {
	{-1, 0, 0},
	{0, 1, 0},
	{0, 0, -1}};

static const struct device *magnetometer;
static bool magnetometer_is_init = false;

static const struct device *gyroscope;
static bool gyroscope_is_init = false;

/* Fifth order polynomial approximation giving 0.05 deg max error */
const int16_t K1 = 5701;
const int16_t K2 = -1645;
const int16_t K3 = 446;

/* Final step size for iDivide */
const uint16_t MINDELTADIV = 1;

/**
 * @brief Calculates the integer division of iy by ix where iy <= ix.
 *
 * This function performs a binary division to calculate the ratio of two integers iy and ix
 * with the constraint that iy is less than or equal to ix. The result is scaled to the range 0 to 32767.
 *
 * @param[in] iy The numerator of the division (must be <= ix and > 0)
 * @param[in] ix The denominator of the division (must be > 0)
 * @param[out] result The scaled result of the division (0 to 32767)
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_i_divide(int16_t iy, int16_t ix, int16_t *result)
{
	int16_t itmp;	/* Scratch */
	int16_t ir;		/* Result = iy / ix, range 0. to 1., returned in range 0 to 32767 */
	int16_t idelta; /* Delta on candidate result, dividing each stage by factor of 2 */

	/* Set result r to zero and binary search step to 16384 = 0.5 */
	ir = 0;
	idelta = 16384; /* Set as 2^14 = 0.5 */

	/* To reduce quantization effects, boost ix and iy to the maximum signed 16 bit value */
	while ((ix < 16384) && (iy < 16384))
	{
		ix += ix;
		iy += iy;
	}

	do /* Loop over binary sub-division algorithm solving for ir*ix = iy */
	{
		/* Generate new candidate solution for ir and test if we are too high or too low */
		itmp = ir + idelta; /* itmp=ir+delta, the candidate solution */
		itmp = (int16_t)((itmp * ix) >> 15);
		if (itmp <= iy)
			ir += idelta;
		idelta >>= 1; /* Divide by 2 using right shift one bit */
	} while (idelta >= MINDELTADIV); /* Last loop is performed for idelta=MINDELTADIV */

	*result = ir;
	return 0;
}

/**
 * @brief Calculates 100 * atan(iy / ix) in degrees.
 *
 * This function calculates 100 times the arctangent of the ratio iy/ix in degrees.
 * It returns a value between 0 and 9000 corresponding to angles from 0 to 90 degrees.
 *
 * @param[in] iy The y component
 * @param[in] ix The x component
 * @param[out] result The arctangent of iy/ix scaled by 100, in the range 0 to 9000.
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_i_hundred_atan_deg(int16_t iy, int16_t ix, int16_t *result)
{
	int32_t iAngle; /* Angle in degrees times 100 */
	int16_t iRatio; /* Ratio of iy / ix or vice versa */
	int32_t iTmp;	/* Temporary variable */
	uint8_t errorCode;

	/* Check for pathological cases */
	if ((ix == 0) && (iy == 0))
	{
		*result = 0;
		return 0;
	}
	if ((ix == 0) && (iy != 0))
	{
		*result = 9000;
		return 0;
	}

	/* Check for non-pathological cases */
	if (iy <= ix)
	{
		errorCode = gyroCompass_i_divide(iy, ix, &iRatio); /* return a fraction in range 0. to 32767 = 0. to 1. */
		if (errorCode != 0)
		{
			return errorCode;
		}
	}
	else
	{
		/* First, third, and fifth order polynomial approximation */
		errorCode = gyroCompass_i_divide(ix, iy, &iRatio); /* return a fraction in range 0. to 32767 = 0. to 1. */
		if (errorCode != 0)
		{
			return errorCode;
		}
	}

	iAngle = (int32_t)K1 * (int32_t)iRatio;
	iTmp = ((int32_t)iRatio >> 5) * ((int32_t)iRatio >> 5) * ((int32_t)iRatio >> 5);
	iAngle += (iTmp >> 15) * (int32_t)K2;
	iTmp = (iTmp >> 20) * ((int32_t)iRatio >> 5) * ((int32_t)iRatio >> 5);
	iAngle += (iTmp >> 15) * (int32_t)K3;
	iAngle = iAngle >> 15;

	/* Check if above 45 degrees */
	if (iy > ix)
		iAngle = 9000 - iAngle;

	/* For tidiness, limit result to range 0 to 9000 equals 0.0 to 90.0 degrees */
	if (iAngle < 0)
		iAngle = 0;
	if (iAngle > 9000)
		iAngle = 9000;

	*result = (int16_t)iAngle;
	return 0;
}

/**
 * @brief Calculates 100 * atan2(iy, ix) in degrees.
 *
 * This function calculates 100 times the arctangent of iy/ix using the atan2 function.
 * It returns a value between -18000 and 18000 corresponding to angles from -180 to 180 degrees.
 *
 * @param[in] iy The y component
 * @param[in] ix The x component
 * @param[out] result The arctangent of iy/ix scaled by 100, in the range -18000 to 18000.
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_i_hundred_atan2_deg(int16_t iy, int16_t ix, int16_t *result)
{
	int16_t iAngle; /* angle in degrees times 100 */
	uint8_t errorCode;

	/* Check for -32768 which is not handled correctly */
	if (ix == -32768)
	{
		ix = -32767;
	}
	if (iy == -32768)
	{
		iy = -32767;
	}

	/* Check for quadrants */
	if ((ix >= 0) && (iy >= 0)) /* Range 0 to 90 degrees */
	{
		errorCode = gyroCompass_i_hundred_atan_deg(iy, ix, &iAngle);
		if (errorCode != 0)
		{
			return errorCode;
		}
	}
	else if ((ix <= 0) && (iy >= 0)) /* Range 90 to 180 degrees */
	{
		errorCode = gyroCompass_i_hundred_atan_deg(iy, (int16_t)-ix, &iAngle);
		if (errorCode != 0)
		{
			return errorCode;
		}
		iAngle = (int16_t)(18000 - iAngle);
	}
	else if ((ix <= 0) && (iy <= 0)) /* Range -180 to -90 degrees */
	{
		errorCode = gyroCompass_i_hundred_atan_deg((int16_t)-iy, (int16_t)-ix, &iAngle);
		if (errorCode != 0)
		{
			return errorCode;
		}
		iAngle = (int16_t)((int16_t)-18000 + iAngle);
	}
	else /* ix >=0 and iy <= 0 giving range -90 to 0 degrees */
	{
		errorCode = gyroCompass_i_hundred_atan_deg((int16_t)-iy, ix, &iAngle);
		if (errorCode != 0)
		{
			return errorCode;
		}
		iAngle = (int16_t)(-iAngle);
	}

	*result = iAngle;
	return 0;
}

/**
 * @brief Calculates ix / sqrt(ix*ix + iy*iy) using binary division.
 *
 * This function computes the ratio ix / sqrt(ix*ix + iy*iy) using a binary division algorithm.
 * The result is returned as a signed 16-bit integer in the range -32768 to 32767.
 *
 * @param[in] ix The x component
 * @param[in] iy The y component
 * @param[out] result The ratio ix / sqrt(ix*ix + iy*iy) as a signed 16-bit integer.
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_i_trig(int16_t ix, int16_t iy, int16_t *result)
{
	uint32_t itmp;	 /* Scratch */
	uint32_t ixsq;	 /* ix * ix */
	int16_t isignx;	 /* Storage for sign of x. Algorithm assumes x >= 0 then corrects later */
	uint32_t ihypsq; /* (ix * ix) + (iy * iy) */
	int16_t ir;		 /* Result = ix / sqrt(ix*ix+iy*iy) range -1, 1 returned as signed int16 */
	int16_t idelta;	 /* Delta on candidate result dividing each stage by factor of 2 */

	/* correct for pathological case: ix==iy==0 */
	if ((ix == 0) && (iy == 0))
	{
		ix = iy = 1;
	}

	/* check for -32768 which is not handled correctly */
	if (ix == -32768)
	{
		ix = -32767;
	}
	if (iy == -32768)
	{
		iy = -32767;
	}

	/* store the sign for later use. algorithm assumes x is positive for convenience */
	isignx = 1;
	if (ix < 0)
	{
		ix = (int16_t)-ix;
		isignx = -1;
	}

	/* for convenience in the boosting set iy to be positive as well as ix */
	iy = (int16_t)abs(iy);

	/* to reduce quantization effects, boost ix and iy but keep below maximum signed 16 bit */
	while ((ix < 16384) && (iy < 16384))
	{
		ix += ix;
		iy += iy;
	}

	/* calculate ix*ix and the hypotenuse squared */
	ixsq = (uint32_t)((uint32_t)ix * (uint32_t)ix);			 /* ixsq=ix*ix: 0 to 32767^2 = 1073676289 */
	ihypsq = (uint32_t)(ixsq + (uint32_t)iy * (uint32_t)iy); /* ihypsq=(ix*ix+iy*iy) 0 to 2*32767*32767=2147352578 */

	/* set result r to zero and binary search step to 16384 = 0.5 */
	ir = 0;
	idelta = 16384; /* Set as 2^14 = 0.5 */

	/* loop over binary sub-division algorithm */
	do
	{
		/* generate new candidate solution for ir and test if we are too high or too low */
		/* itmp=(ir+delta)^2, range 0 to 32767*32767 = 2^30 = 1073676289 */
		itmp = (uint32_t)((int32_t)(ir + idelta) * (int32_t)(ir + idelta)); /* itmp=(ir+idelta)^2 */

		/* itmp=(ir+delta)^2*(ix*ix+iy*iy), range 0 to 2^31 = 2147221516 */
		itmp = (itmp >> 15) * (ihypsq >> 15);
		if (itmp <= ixsq)
			ir += idelta;
		idelta >>= 1; /* divide by 2 using right shift one bit */
	} while (idelta >= MINDELTATRIG); /* last loop is performed for idelta=MINDELTATRIG */

	/* correct the sign */
	*result = (int16_t)(ir * isignx);
	return 0;
}

/**
 * @brief Computes the yaw angle.
 *
 * This function calculates the yaw angle based on the magnetometer and accelerometer data.
 * The computed yaw angle is returned through the provided angle parameter.
 *
 * @param[in] iBpx Magnetometer x-axis data
 * @param[in] iBpy Magnetometer y-axis data
 * @param[in] iBpz Magnetometer z-axis data
 * @param[in] iGpx Accelerometer x-axis data
 * @param[in] iGpy Accelerometer y-axis data
 * @param[in] iGpz Accelerometer z-axis data
 * @param[out] angle The computed yaw angle
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_i_ecompass(int16_t iBpx, int16_t iBpy, int16_t iBpz,
									  int16_t iGpx, int16_t iGpy, int16_t iGpz, double *angle)
{
	int16_t iSin, iCos; /* sine and cosine */
	int16_t iPhi_16t, iThe; /* roll and pitch angles */
	int iPhi;
	int16_t iBfy, iBfx, iBfz;
	uint8_t errorCode;

	errorCode = gyroCompass_i_hundred_atan2_deg(iGpy, iGpz, &iPhi_16t); /* Eq 13 */
	if (errorCode != 0)
	{
		return errorCode;
	}
	iPhi = iPhi_16t;
	iPhi -= 18000;	  // 180 degree offset
	if (iPhi > 18000) // make sure it stays between -180 and 180 degrees
	{
		iPhi -= 36000;
	}
	if (iPhi < -18000)
	{
		iPhi += 36000;
	}

	/* calculate sin and cosine of roll angle Phi */
	errorCode = gyroCompass_i_trig(iGpy, iGpz, &iSin); /* Eq 13: sin = opposite / hypotenuse */
	if (errorCode != 0)
	{
		return errorCode;
	}
	errorCode = gyroCompass_i_trig(iGpz, iGpy, &iCos); /* Eq 13: cos = adjacent / hypotenuse */
	if (errorCode != 0)
	{
		return errorCode;
	}

	/* de-rotate by roll angle Phi */
	iBfy = (int16_t)((iBpy * iCos - iBpz * iSin) >> 15); /* Eq 19 y component */
	iBpz = (int16_t)((iBpy * iSin + iBpz * iCos) >> 15); /* Bpy*sin(Phi)+Bpz*cos(Phi)*/
	iGpz = (int16_t)((iGpy * iSin + iGpz * iCos) >> 15); /* Eq 15 denominator */

	/* calculate current pitch angle Theta */
	errorCode = gyroCompass_i_hundred_atan2_deg((int16_t)-iGpx, iGpz, &iThe); /* Eq 15 */
	if (errorCode != 0)
	{
		return errorCode;
	}

	if (iThe > 9000) // make sure it stays between -90 and 90 degrees
	{
		iThe = (int16_t)(18000 - iThe);
	}
	if (iThe < -9000)
	{
		iThe = (int16_t)(-18000 - iThe);
	}

	/* calculate sin and cosine of pitch angle Theta */
	errorCode = gyroCompass_i_trig(iGpx, iGpz, &iSin); /* Eq 15: sin = opposite / hypotenuse */
	if (errorCode != 0)
	{
		return errorCode;
	}
	iSin = (int16_t)-iSin;
	errorCode = gyroCompass_i_trig(iGpz, iGpx, &iCos); /* Eq 15: cos = adjacent / hypotenuse */
	if (errorCode != 0)
	{
		return errorCode;
	}

	/* correct cosine if pitch not in range -90 to 90 degrees */
	if (iCos < 0)
	{
		iCos = (int16_t)-iCos;
	}

	iBfx = (int16_t)((iBpx * iCos + iBpz * iSin) >> 15);  /* Eq 19: x component */
	iBfz = (int16_t)((-iBpx * iSin + iBpz * iCos) >> 15); /* Eq 19: z component */

	*angle = atan2(iBfy, iBfx) * 180 / M_PIL;

	*angle += 180; // Add 180 degrees offset

	/* restrict yaw angle to range 0 to 360 degrees */
	if (*angle > 360)
	{
		*angle -= 360;
	}
	else if (*angle < 0)
	{
		*angle += 360;
	}

	return 0;
}

/**
 * @brief Multiplies two matrices.
 *
 * This function performs matrix multiplication of a 3x3 matrix and a 3x1 vector.
 * The result is stored in the provided result array.
 *
 * @param[out] result The resulting 3x1 vector from the multiplication
 * @param[in] matrix1 The 3x3 matrix
 * @param[in] matrix2 The 3x1 vector
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_multiply_matrices(int16_t result[3], const int16_t matrix1[3][3], const int16_t matrix2[3])
{
	int i, j;
	for (i = 0; i < 3; ++i)
	{
		result[i] = 0;
		for (j = 0; j < 3; ++j)
		{
			result[i] += matrix1[i][j] * matrix2[j];
		}
	}
	return 0;
}

/**
 * @brief Rotates the x, y, and z axis data.
 *
 * This function applies rotation matrices to the magnetometer and accelerometer data to adjust their axes.
 *
 * @param[in, out] aMagnoData The 3x1 vector for magnetometer data, will be rotated in place
 * @param[in, out] aAccelData The 3x1 vector for accelerometer data, will be rotated in place
 *
 * @return Error code (0 for success)
 */
static uint8_t gyroCompass_rotate_data(int16_t aMagnoData[3], int16_t aAccelData[3])
{
	uint16_t tempResult[3];
	uint8_t errorCode;

	errorCode = gyroCompass_multiply_matrices(tempResult, rotation_magno, aMagnoData);
	if (errorCode != 0)
	{
		return errorCode;
	}
	for (int i = 0; i < 3; i++)
	{
		aMagnoData[i] = tempResult[i];
	}

	errorCode = gyroCompass_multiply_matrices(tempResult, rotation_accel, aAccelData);
	if (errorCode != 0)
	{
		return errorCode;
	}
	for (int i = 0; i < 3; i++)
	{
		aAccelData[i] = tempResult[i];
	}

	return 0;
}

/**
 * @brief Sets the sampling frequency for the gyroscope.
 *
 * This function sets the sampling frequency for the gyroscope sensor.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 10: Cannot set sampling frequency for accelerometer
 */
uint8_t magnetometer_set_sampling_freq(int aFreq)
{
	int error = 0;
	struct sensor_value odr_attr;

	odr_attr.val1 = aFreq;
	odr_attr.val2 = 0;

	error = sensor_attr_set(magnetometer, SENSOR_CHAN_MAGN_XYZ,
							SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
	if (error != 0)
	{
		printf("Cannot set sampling frequency for magnetometer.\n");
		return 10;
	}

	return 0;
}

/**
 * @brief Initializes the magnetometer sensor.
 *
 * This function initializes the LIS3MDL magnetometer sensor and sets up
 * any necessary trigger for data ready events.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 1: Failed to get LIS3MDL device
 *         - 2: Magnetometer already initialized
 *         - Errors from magnetometer_set_sampling_freq()
 */
uint8_t magnetometer_init(void)
{
	if (magnetometer_is_init)
	{
		printf("Magnetometer already initialized\n");
		return 2;
	}

	magnetometer = DEVICE_DT_GET(DT_ALIAS(magnetometer));
	if (!device_is_ready(magnetometer))
	{
		printf("Could not get LIS3MDL device\n");
		return 1;
	}

	// int error = magnetometer_set_sampling_freq(magno_sample_freq);
	// if (error != 0)
	// {
	//  return error;
	// }

	magnetometer_is_init = true;
	return 0;
}

/**
 * @brief Exits the magnetometer sensor.
 *
 * This function stops the magnetometer sensor and releases any resources
 * associated with it.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 1: Magnetometer not initialized
 *         - Errors from magnetometer_set_sampling_freq()
 */
uint8_t magnetometer_exit(void)
{
	if (!magnetometer_is_init)
	{
		return 1;
	}

	// int error = magnetometer_set_sampling_freq(magno_sample_freq_off);
	// if (error != 0)
	// {
	//  return error;
	// }

	magnetometer_is_init = false;
	return 0;
}

/**
 * @brief Retrieves the heading from the magnetometer sensor.
 *
 * This function fetches the latest magnetometer data, processes it to
 * calculate the heading, and returns the result.
 *
 * @param aHeading[out] Pointer to a variable where the heading will be stored.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 1: Magnetometer not initialized
 *         - 2: LIS3MDL Sensor sample update error
 */
uint8_t magnetometer_get_magneto(int16_t *aMagneto)
{
	struct sensor_value magn_xyz[3];
	double magn_xyz_double[3];

	if (!magnetometer_is_init)
	{
		printf("Magnetometer not initialized\n");
		return 1;
	}

	sensor_sample_fetch_chan(magnetometer, SENSOR_CHAN_MAGN_XYZ);
	sensor_channel_get(magnetometer, SENSOR_CHAN_MAGN_XYZ, &magn_xyz[0]);

	// Change value to float
	for (int i = 0; i < 3; i++)
	{
		magn_xyz_double[i] = sensor_value_to_double(&magn_xyz[i]);
	}

	// Subtract the hard iron offset
	magn_xyz_double[0] -= X_OFFSET;
	magn_xyz_double[1] -= Y_OFFSET;
	magn_xyz_double[2] -= Z_OFFSET;

	// Scale value over max range of int. Value is max 1 or -1
	for (int i = 0; i < 3; i++)
	{
		aMagneto[i] = (int16_t)(magn_xyz_double[i] * 32767);
	}
	return 0;
}

/**
 * @brief Sets the sampling frequency for the gyroscope.
 *
 * This function sets the sampling frequency for the gyroscope sensor.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 10: Cannot set sampling frequency for accelerometer
 *         - 20: Cannot set sampling frequency for gyro
 */
uint8_t gyroscope_set_sampling_freq(int aFreq)
{
	int error = 0;
	struct sensor_value odr_attr;

	/* set accel/gyro sampling frequency to 12.5 Hz */
	odr_attr.val1 = aFreq;
	odr_attr.val2 = 0;

	error = sensor_attr_set(gyroscope, SENSOR_CHAN_ACCEL_XYZ,
							SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
	if (error != 0)
	{
		printf("Cannot set sampling frequency for accelerometer.\n");
		return 10;
	}

	error = sensor_attr_set(gyroscope, SENSOR_CHAN_GYRO_XYZ,
							SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
	if (error != 0)
	{
		printf("Cannot set sampling frequency for gyro.\n");
		return 20;
	}

	return 0;
}

/**
 * @brief Initializes the gyroscope sensor.
 *
 * This function initializes the gyroscope sensor and sets the sampling frequency.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 1: Device not ready
 *         - 2: Gyroscope already initialized
 *         - Errors from gyroscope_set_sampling_freq()
 */
uint8_t gyroscope_init(void)
{
	if (gyroscope_is_init)
	{
		printf("Magnetometer already initialized\n");
		return 2;
	}

	gyroscope = DEVICE_DT_GET(DT_ALIAS(gyroscope));
	int error = 0;

	if (!device_is_ready(gyroscope))
	{
		printk("%s: device not ready.\n", gyroscope->name);
		return 1;
	}

	error = gyroscope_set_sampling_freq(gyro_sample_freq);
	if (error != 0)
	{
		return error;
	}

	gyroscope_is_init = true;
	return 0;
}

/**
 * @brief Exits the gyroscope sensor.
 *
 * This function stops the gyroscope sensor.
 *
 * @return 0 if successful, otherwise a positive error code:
 *         - 1: Device is not initialized
 *         - Errors from gyroscope_set_sampling_freq()
 */
uint8_t gyroscope_exit(void)
{
	if (!gyroscope_is_init)
	{
		printf("Gyroscope not initialized\n");
		return 1;
	}

	int error = gyroscope_set_sampling_freq(zero);
	if (error != 0)
	{
		return error;
	}

	gyroscope_is_init = false;

	return 0;
}

/**
 * @brief Retrieves the acceleration from the gyroscope sensor.
 *
 * This function fetches the latest acceleration data from the gyroscope sensor.
 *
 * @param aAcceleration[out] Pointer to a variable where the acceleration will be stored. Expects a double[3] array.
 *
 * @return 0 if successful, 1 if the gyroscope was not initialized.
 */
uint8_t gyroscope_get_acceleration(int16_t aAcceleration[3])
{
	if (!gyroscope_is_init)
	{
		printf("Gyroscope not initialized\n");
		return 1;
	}

	struct sensor_value gyro_xyz[3];

	sensor_sample_fetch_chan(gyroscope, SENSOR_CHAN_ACCEL_XYZ);
	sensor_channel_get(gyroscope, SENSOR_CHAN_ACCEL_XYZ, &gyro_xyz[0]);

	for (int i = 0; i < 3; i++)
	{
		float floatValue = sensor_value_to_float(&gyro_xyz[i]);
		aAcceleration[i] = (double)floatValue / 19.6 * 32767;
	}

	return 0;
}

/**
 * @brief Retrieves the angular velocity from the gyroscope sensor.
 *
 * This function fetches the latest angular velocity data from the gyroscope sensor.
 *
 * @param aGyro[out] Pointer to a variable where the angular velocity will be stored. Expects a double[3] array.
 *
 * @return 0 if successful, 1 if the gyroscope was not initialized.
 */
uint8_t gyroscope_get_gyro(float aGyro[3])
{
	if (!gyroscope_is_init)
	{
		printf("Gyroscope not initialized\n");
		return 1;
	}

	struct sensor_value gyro_xyz[3];
	sensor_sample_fetch_chan(gyroscope, SENSOR_CHAN_GYRO_XYZ);
	sensor_channel_get(gyroscope, SENSOR_CHAN_GYRO_XYZ, &gyro_xyz[0]);

	for (int i = 0; i < 3; i++)
	{
		aGyro[i] = sensor_value_to_float(&gyro_xyz[i]);
	}

	return 0;
}

/**
 * @brief Retrieves the roll angle from the gyroscope sensor.
 *
 * This function calculates the roll angle using the gyroscope's acceleration data.
 *
 * @param aRoll[out] Pointer to a variable where the roll angle will be stored. Value is in degrees
 *
 * @return 0 if successful, 1 if the gyroscope was not initialized, or error code if there is an error in getting acceleration or calculating the roll.
 */
uint8_t gyroscope_get_roll(int *aRoll)
{
	int16_t acceleration[3];
	int errorCode = 0;
	int16_t roll;
	uint16_t tempResult[3];
	if (!gyroscope_is_init)
	{
		printf("Gyroscope not initialized\n");
		return 1;
	}

	errorCode = gyroscope_get_acceleration(acceleration);
	if (errorCode > 0)
	{
		return errorCode;
	}

	errorCode = gyroCompass_multiply_matrices(tempResult, rotation_magno, acceleration);
	if (errorCode != 0)
	{
		return errorCode;
	}
	for (int i = 0; i < 3; i++)
	{
		acceleration[i] = tempResult[i];
	}
	errorCode = gyroCompass_i_hundred_atan2_deg(acceleration[1], acceleration[2], &roll); /* Eq 13 */
	if (errorCode != 0)
	{
		return errorCode;
	}
	*aRoll = roll;
	*aRoll -= 18000;	  // 180 degree offset
	if (*aRoll > 18000) // make sure it stays between -180 and 180 degrees
	{
		*aRoll -= 36000;
	}
	if (*aRoll < -18000)
	{
		*aRoll += 36000;
	}

	*aRoll = *aRoll / 100;

	return 0;
}

/**
 * @brief Retrieves the pitch angle from the gyroscope sensor.
 *
 * This function calculates the pitch angle using the gyroscope's acceleration data.
 *
 * @param aPitch[out] Pointer to a variable where the pitch angle will be stored. Value is in degrees
 *
 * @return 0 if successful, 1 if the gyroscope was not initialized, or error code if there is an error in getting acceleration or calculating the pitch.
 */
uint8_t gyroscope_get_pitch(int *aPitch)
{
	int16_t acceleration[3];
	int errorCode = 0;
	int16_t pitch;
	int16_t tempResult[3];
	if (!gyroscope_is_init)
	{
		printf("Gyroscope not initialized\n");
		return 1;
	}

	errorCode = gyroscope_get_acceleration(acceleration);
	if (errorCode > 0)
	{
		return errorCode;
	}
	errorCode = gyroCompass_multiply_matrices(tempResult, rotation_magno, acceleration);
	if (errorCode != 0)
	{
		return errorCode;
	}
	for (int i = 0; i < 3; i++)
	{
		acceleration[i] = tempResult[i];
	}
	errorCode = gyroCompass_i_hundred_atan2_deg((int16_t)-acceleration[0], acceleration[2], &pitch);
	if (errorCode != 0)
	{
		return errorCode;
	}

	if (pitch > 9000) // make sure it stays between -90 and 90 degrees
	{
		pitch = (int16_t)(18000 - pitch);
	}
	if (pitch < -9000)
	{
		pitch = (int16_t)(-18000 - pitch);
	}

	*aPitch = pitch / 100;

	return 0;
}

/**
 * @brief Retrieves the heading from the gyroscope and magnetometer sensors.
 *
 * This function calculates the heading using data from both the gyroscope and the magnetometer sensors.
 *
 * @param aHeading[out] Pointer to a variable where the heading will be stored. Expects a int variable. Value is in degress
 *
 * @return 0 if successful.
 * 			1 if gyroscope is not initialized.
 * 			2 if magnetometer is not initialized.
 * 			3 if function error
 */
uint8_t gyroCompass_get_heading(int *aHeading)
{
	int16_t MagnetoValue[3], AccelValue[3];
	double angle;
	int errorCode = 0;
	if (!gyroscope_is_init)
	{
		printf("Gyroscope not initialized\n");
		return 1;
	}
	if (!magnetometer_is_init)
	{
		printf("Magnetometer not initialized\n");
		return 2;
	}
	errorCode = magnetometer_get_magneto(MagnetoValue);
	if (errorCode)
	{
		return 3;
	}

	errorCode = gyroscope_get_acceleration(AccelValue);
	if (errorCode)
	{
		return 3;
	}
	errorCode = gyroCompass_rotate_data(MagnetoValue, AccelValue);
	if (errorCode)
	{
		return 3;
	}

	errorCode = gyroCompass_i_ecompass(MagnetoValue[0], MagnetoValue[1], MagnetoValue[2], AccelValue[0], AccelValue[1], AccelValue[2], &angle);
	if (errorCode)
	{
		return 3;
	}

	*aHeading = angle;

	return 0;
}
