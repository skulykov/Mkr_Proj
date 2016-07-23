#ifndef MapUtils_H
#define MapUtils_H


#define _USE_MATH_DEFINES // for C++
#include <math.h>
#include <float.h>
#include "common.h"


class MapUtils{
	static double toRadians(double val)
	{
		return M_PI*val/180.0;
	}
public:
	static double checkLongitude(double longitude) {
		while (longitude < -180 || longitude > 180) {
			if (longitude < 0) {
				longitude += 360;
			} else {
				longitude -= 360;
			}
		}
		return longitude;
	}
	
	static double checkLatitude(double latitude) {
		while (latitude < -90 || latitude > 90) {
			if (latitude < 0) {
				latitude += 180;
			} else {
				latitude -= 180;
			}
		}
		if(latitude < -85.0511) {
			return -85.0511;
 		} else if(latitude > 85.0511){
 			return 85.0511;
 		}
		return latitude;
	}
	static int get31TileNumberX(double longitude){
		longitude = checkLongitude(longitude);
		unsigned int l = 1l << 31;
		return (int)((longitude + 180)/360 * l);
	}
	static int get31TileNumberY( double latitude){
		latitude = checkLatitude(latitude);
		double eval = log( tan(toRadians(latitude)) + 1/cos(toRadians(latitude)) );
		unsigned int l = 1l << 31;
		if(eval > M_PI){
			eval = M_PI;
		}
		return  (int) ((1 - eval / M_PI) / 2 * l);
	}


	static double get31LongitudeX(int tileX){
		return getLongitudeFromTile(21, tileX /1024.0);
	}
	
	static double get31LatitudeY(int tileY){
		return getLatitudeFromTile(21, tileY /1024.0);
	}

	static double getLongitudeFromTile(float zoom, double x) {
		return x / getPowZoom(zoom) * 360.0 - 180.0;
	}
	
	static double getPowZoom(float zoom){
		if(zoom >= 0 && zoom - floor(zoom) < 0.05f){
			return 1 << ((int)zoom); 
		} else {
			return pow(2, zoom);
		}
	}

	static double getLatitudeFromTile(float zoom, double y){
		int sign = y < 0 ? -1 : 1;
		double result = atan(sign*sinh(M_PI * (1 - 2 * y / getPowZoom(zoom)))) * 180 / M_PI;
		return result;
	}

	//--------------------------------------------------------------------------------
	static double GetTileNumberX(float zoom, Sk_float /*double*/ longitude){
		/*
		if(longitude == 180) {
			return getPowZoom(zoom) - 1;
		}
		longitude = checkLongitude(longitude);
		return (longitude + 180)/360 * getPowZoom(zoom);
		*/
		
        return getTileNumberX(zoom, longitude);
	}
	
	static double GetTileNumberY(float zoom,  Sk_float /*double*/ latitude){
		/*
		latitude = checkLatitude(latitude);
		double eval = log( tan(toRadians(latitude)) + 1/cos(toRadians(latitude)) );
		if (!_finite(eval) || _isnan(eval)) {
			latitude = latitude < 0 ? - 89.9 : 89.9;
			eval = log( tan(toRadians(latitude)) + 1/cos(toRadians(latitude)) );
		}
		double result = (1 - eval / M_PI) / 2 * getPowZoom(zoom);
		return  result;
*/
		return getTileNumberY(zoom, (float) latitude);
	}
	//-------------------------------------------------------------------------------------


	/**
	 * Gets distance in meters
	 */
	static double getDistance(double lat1, double lon1, double lat2, double lon2){
		double R = 6371; // km
		double dLat = toRadians(lat2-lat1);
		double dLon = toRadians(lon2-lon1); 
		double a = sin(dLat/2) * sin(dLat/2) +
		        cos(toRadians(lat1)) * cos(toRadians(lat2)) * 
		        sin(dLon/2) * sin(dLon/2); 
		double c = 2 * atan2(sqrt(a), sqrt(1-a)); 
		return R * c * 1000;
	}
	
	
	/**
	 * Gets distance in meters
	 */
	/*static double getDistance(LatLon l1, LatLon l2){
		return getDistance(l1.getLatitude(), l1.getLongitude(), l2.getLatitude(), l2.getLongitude());
	}*/


	static double alignAngleDifference(double diff) {
		while(diff > M_PI) {
			diff -= 2 * M_PI;
		}
		while(diff <=-M_PI) {
			diff += 2 * M_PI;
		}
		return diff;
		
	}
	static double degreesDiff(double a1, double a2){
		double diff = a1 - a2;
		while(diff > 180) {
			diff -= 360;
		}
		while(diff <=-180) {
			diff += 360;
		}
		return diff;

	}
	static double normalizedAngleRadians( double angle )
	{
		while(angle > M_PI)
			angle -= 2.0 * M_PI;
		while(angle <= -M_PI)
			angle += 2.0 * M_PI;
		return angle;
	}

	static  double normalizedAngleDegrees( double angle )
	{
		while(angle > 180.0)
			angle -= 360.0;
		while(angle <= -180.0)
			angle += 360.;
		return angle;
	}
	static double x31toMeters( int32_t x31 )
	{
		return static_cast<double>(x31) * 0.011;
	}

	static double y31toMeters( int32_t y31 )
	{
		return static_cast<double>(y31) * 0.01863;
	}
};


#endif
