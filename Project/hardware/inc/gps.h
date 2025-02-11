#include <stdint.h>
#include <zephyr/drivers/gnss.h>

void tgps(void);

struct gnss_data getGnssData();
int64_t getLatitude();
int64_t getLongitude();
long double getDistanceMeters(long double lat1, long double long1, long double lat2, long double long2);
long double nanoDegToLdDeg(int64_t);
long double getAngle(long double , long double, long double, long double);
long double toRadians(const long double radians);
long double toDegrees(const long double radians);