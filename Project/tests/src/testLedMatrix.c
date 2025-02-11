#include "testLedMatrix.h"


#ifdef LED_MATRIX
ZTEST_SUITE(testLedMatrix, NULL, NULL, NULL, NULL, NULL);

ZTEST(testLedMatrix, test_Config_one)
{
	bool a =  ledMatrixConfig();
	zassert_equal(a,0);
}

ZTEST(testLedMatrix, test_Init_one)
{	
	int8_t a = ledMatrixInit();
	zassert_equal(a,0);
}

ZTEST(testLedMatrix, test_Set_one)
{
	int16_t data[16] = {0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 
						0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
						0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
						0b1111111111111111, 0b1111111111111111, 0b1111111111111111, 0b1111111111111111};
	int8_t a = ledMatrixSet(data);
	zassert_equal(a,0);
}

#endif //LED_MATRIX