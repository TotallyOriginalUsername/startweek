#include "testCircleMatrix.h"

#ifdef CIRCLE_MATRIX

ZTEST_SUITE(testCircleMatrix, NULL, NULL, NULL, NULL, NULL);

ZTEST(testCircleMatrix, test_Config_one)
{
	bool a =  circleMatrixConfig();
	zassert_equal(a,0);
}

ZTEST(testCircleMatrix, test_Init_one)
{	
	int8_t a = circleMatrixInit();
	zassert_equal(a,0);
}

ZTEST(testCircleMatrix, test_Set_one)
{
	int8_t data[8] = 	{0b11111111, 0b11111111, 0b11111111, 0b11111111, 
						0b11111111, 0b11111111, 0b11111111, 0b11111111,
						};
	int8_t a = circleMatrixSet(data);
	zassert_equal(a,0);
}

#endif //CIRCLE_MATRIX