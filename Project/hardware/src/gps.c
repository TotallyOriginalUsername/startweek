#include "gps.h"
#include "lcd.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gnss.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

# define M_PI		3.14159265358979323846	/* pi */

struct gnss_data gpsData;

static void gnss_data_cb(const struct device *dev, const struct gnss_data *data)
{
	if (data->info.fix_status != GNSS_FIX_STATUS_NO_FIX) {
		gpsData = *data;
	}
}

struct gnss_data getGnssData() {
	return gpsData;
}

int64_t getLatitude() {
	return gpsData.nav_data.latitude;
}

int64_t getLongitude() {
	return gpsData.nav_data.longitude;
}

GNSS_DATA_CALLBACK_DEFINE(DEVICE_DT_GET(DT_ALIAS(gnss)), gnss_data_cb);

#if CONFIG_GNSS_SATELLITES
static void gnss_satellites_cb(const struct device *dev, const struct gnss_satellite *satellites,
			       uint16_t size)
{
	//printf("%s reported %u satellites!\r\n", dev->name, size);
	//lcdClear();
	//char words[27];
	//char value[3];
	//sprintf(value, "%d", size);
	////value[0] = (char)(size + '0');
	//strcpy(words,"Nr. of satellites:");
	//strcat(words, value);
	//lcdStringWrite(words);
	//k_busy_wait(3000000);	
}
#endif

GNSS_SATELLITES_CALLBACK_DEFINE(DEVICE_DT_GET(DT_ALIAS(gnss)), gnss_satellites_cb);

// Utility function for 
// converting degrees to radians
long double toRadians(const long double degree)
{
    // cmath library in C++ 
    // defines the constant
    // M_PI as the value of
    // pi accurate to 1e-30
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}

// Utility function for 
// converting radians to degrees
long double toDegrees(const long double radians)
{
    long double one_rad = 180 / (M_PI);
    return (one_rad * radians);
}
 
long double getDistanceMeters(long double lat1, long double long1, 
                     long double lat2, long double long2)
{
    // Convert the latitudes 
    // and longitudes
    // from degree to radians.
    lat1 = toRadians(lat1);
    long1 = toRadians(long1);
    lat2 = toRadians(lat2);
    long2 = toRadians(long2);
     
    // Haversine Formula
    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;
 
    long double ans = pow(sin(dlat / 2), 2) + 
                          cos(lat1) * cos(lat2) * 
                          pow(sin(dlong / 2), 2);
 
    ans = 2 * asin(sqrt(ans));
 
    // Radius of Earth in 
    // Kilometers, R = 6371
    // Use R = 3956 for miles
    long double R = 6371;
     
    // Calculate the result
    ans = ans * R;

	ans *= 1000;	// Convert km to m
 
    return ans;
}

long double nanoDegToLdDeg(int64_t value) {
	return (long double)value/1000000000;
}

/** 
 * @brief Get the angle between two coordinates
 * 
 * Calculate the angle between coordinates in degrees. Takes a latitude and longitude (twice) for two coordinates.
 * For example, if the second coordinate is directly east of the first coordinate, it returns 90.
 * If the second coordinate is directly northwest of the first coordinate, it returns 315.
 * 
 * @return Angle between the coordinate arguments in degrees
 */ 
long double getAngle(long double lat1, long double lon1, long double lat2, long double lon2) {
	lat1 = toRadians(lat1);
	lon1 = toRadians(lon1);
	lat2 = toRadians(lat2);
	lon2 = toRadians(lon2);
    long double dLon = (lon2 - lon1);

    long double y = sin(dLon) * cos(lat2);
    long double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

    long double brng = atan2(y, x);

    brng = toDegrees(brng);
    brng = fmod((brng + 360), 360);
    //brng = 360 - brng; // count degrees counter-clockwise - remove to make clockwise

    return brng;
}