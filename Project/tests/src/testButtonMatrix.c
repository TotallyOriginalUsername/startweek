#include "testButtonMatrix.h"


#ifdef BUTTON_MATRIX
ZTEST_SUITE(testButtonMatrix, NULL, NULL, NULL, NULL, NULL);

ZTEST(testButtonMatrix, test_Matrix_Config_one)
{
	bool a =  buttonMatrixConfig();
	zassert_equal(a,0);
}

ZTEST(testButtonMatrix, test_Matrix_Init_one)
{	
	int8_t a = buttonMatrixInit();
	zassert_equal(a,0);
}

ZTEST(testButtonMatrix, test_Matrix_Set_one)
{
	int8_t data[4] = {0b11111111, 0b11111111, 0b11111111, 0b11111111};
	int8_t a = buttonMatrixSet(data);
	zassert_equal(a,0);
}

ZTEST(testButtonMatrix, test_Buttons_Config_one)
{
	bool a =  buttons4x4Config();
	zassert_equal(a,0);
}

ZTEST(testButtonMatrix, test_Buttons_Init_one)
{	
	int8_t a = buttons4x4Init();
	zassert_equal(a,0);
}

ZTEST(testButtonMatrix, test_Buttons_Get_one)
{
	for(uint8_t i = 0; i < 16; i++)
	{
		uint8_t a = buttons4x4Get(i);
		zassert_equal(a,1);
	}
}



#endif //BUTTON_MATRIX