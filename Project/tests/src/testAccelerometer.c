#include "testGyroCompass.h"

ZTEST_SUITE(testAccelerometer, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(testMagnetometer, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(testCompass, NULL, NULL, NULL, NULL, NULL);

ZTEST(testAccelerometer, test_accelerometer_init)
{
	bool a =  gyroscope_init();
	zassert_equal(a,0);
}

ZTEST(testAccelerometer, test_accelerometer_exit)
{
	bool a =  gyroscope_init();
	zassert_equal(a,0);
	a =  gyroscope_exit();
	zassert_equal(a,0);
}

ZTEST(testAccelerometer, test_accelerometer_get_acceleration)
{
	int16_t tempData[3];
	bool a =  gyroscop_init();
	zassert_equal(a,0);
	a = gyroscope_get_acceleration(tempData);
	zassert_equal(a,0);
}

ZTEST(testAccelerometer, test_magnetometer_get_gyro)
{
	int16_t tempData[3];
	bool a =  gyroscope_init();
	zassert_equal(a,0);
	a = gyroscope_get_gyro(tempData);
	zassert_equal(a,0);
}

ZTEST(testMagnetometer, test_magnetometer_init)
{
	bool a =  magnetometer_init();
	zassert_equal(a,0);
}

ZTEST(testMagnetometer, test_magnetometer_exit)
{
	bool a =  magnetometer_init();
	zassert_equal(a,0);
	a =  magnetometer_exit();
	zassert_equal(a,0);
}

ZTEST(testMagnetometer, test_magnetometer_get_magneto)
{
	int16_t tempData[3];
	bool a =  magnetometer_init();
	zassert_equal(a,0);
	a = magnetometer_get_magneto(tempData);
	zassert_equal(a,0);
}

ZTEST(testCompass, test_compass_get_heading)
{
	double tempData;
	bool a =  magnetometer_init();
	zassert_equal(a,0);
	a = gyroscope_init();
	zassert_equal(a,0);
	a = gyroCompass_get_heading(tempData);
	zassert_equal(a,0);
}
