#include "testSevenSegment.h"
#ifdef SEVEN_SEGMENT

ZTEST_SUITE(sevenSegmentMatrix, NULL, NULL, NULL, NULL, NULL);

ZTEST(sevenSegmentMatrix, test_Config_one)
{
	bool a =  sevenSegmentConfig();
	zassert_equal(a,0);
}

ZTEST(sevenSegmentMatrix, test_Init_one)
{	
	int8_t a = sevenSegmentInit();
	zassert_equal(a,0);
}

ZTEST(sevenSegmentMatrix, test_Set_one)
{
	char msg[4] = "1234";
	uint8_t a = sevenSegmentSet(msg,1);
	zassert_equal(a,0);
}

#endif //SEVEN_SEGMENT
