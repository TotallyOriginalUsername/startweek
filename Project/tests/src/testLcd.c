
#include "testLcd.h"

#ifdef LCD
ZTEST_SUITE(testLcd, NULL, NULL, NULL, NULL, NULL);

ZTEST(testLcd, test_Config_one)
{
	bool a =  lcdConfig();
	zassert_equal(a,0);
}

ZTEST(testLcd, test_Init_one)
{	
	int8_t a = lcdInit();
	zassert_equal(a,0);
}

ZTEST(testLcd, test_Set_one)
{
	char msg[5] = "test";
	uint8_t a = lcdStringWrite(msg);
	zassert_equal(a,0);
	a =  lcdEnable();
	zassert_equal(a,0);
	a =  lcdDisable();
	zassert_equal(a,0);
	a =  lcdClear();
	zassert_equal(a,0);

}

#endif //LCD