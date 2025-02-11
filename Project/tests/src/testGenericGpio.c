#include "testGenericGpio.h"


#ifdef GENERIC_GPIO
int8_t a = 0;
ZTEST_SUITE(testGenericGpio, NULL, NULL, NULL, NULL, NULL);

ZTEST(testGenericGpio, test_Switch_Config_one)
{
	a =  switchesConfig();
	zassert_equal(a,0);
}

ZTEST(testGenericGpio, test_Switch_Init_one)
{	
	a = switchesInit();
	zassert_equal(a,0);
}

ZTEST(testGenericGpio, test_Switch_Get_one)
{

	for(uint8_t i = 0; i < 5; i++)
	{
		a = switchesGet(i);
		zassert_equal(a,0);
	}
}


ZTEST(testGenericGpio, test_abc_Config_one)
{
	a = abcbuttonsConfig();
	zassert_equal(a,0);
	a = abcledsConfig();
	zassert_equal(a,0);
}

ZTEST(testGenericGpio, test_abc_Init_one)
{	
	a = abcbuttonsInit();
	zassert_equal(a,0);
	a = abcledsInit();
	zassert_equal(a,0);
}

ZTEST(testGenericGpio, test_abc_GetSet_one)
{

	a = abcbuttonsGet('a');
	zassert_equal(a,0);
	a = abcledsSet('a',0);
	zassert_equal(a,0);

	a = abcbuttonsGet('b');
	zassert_equal(a,0);
	a = abcledsSet('b',0);
	zassert_equal(a,0);

	a = abcbuttonsGet('c');
	zassert_equal(a,0);
	a = abcledsSet('c',0);
	zassert_equal(a,0);
}



ZTEST(testGenericGpio, test_start_Config_one)
{
	a = startbuttonledConfig();
	zassert_equal(a,0);
}

ZTEST(testGenericGpio, test_start_Init_one)
{	
	a = startbuttonledInit();
	zassert_equal(a,0);

}

ZTEST(testGenericGpio, test_start_GetSet_one)
{
	a = startbuttonGet();
	zassert_equal(a,0);
	a = startledSet(0);
	zassert_equal(a,0);
}

#endif //GENERIC_GPIO
