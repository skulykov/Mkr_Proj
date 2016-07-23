#include "common.h"
#include "dbg.h"

#include <stdarg.h>


#if defined(_WIN32)
#	include <windows.h>
#	include <mmsystem.h>   // svk  replace timeGetTime to avoid use WinMM.lib
#elif defined(__APPLE__)
#	include <mach/mach_time.h>|
#else
#	include <time.h>
#endif


#if LEAK_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
//#endif
#endif


TextDrawInfo::TextDrawInfo(std::string itext)
	: text(itext)
	, drawOnPath(false)
	, path(NULL)
	, pathRotate(0)
{
	visible=0;
	processed_for_test=0;
	openSpace=0;
	centerX = 0;
	centerY = 0;
}

TextDrawInfo::~TextDrawInfo()
{
	if (path)
	{
//#if NEW_APPROACH_WITH_MAP			
//		if(path->arr)
//			delete[] path->arr;  // array first
//#endif		
		delete path;
	}
}

IconDrawInfo::IconDrawInfo()
	: bmp(NULL)
{

}

//RenderingContext::~RenderingContext()
//{
//	std::vector<TextDrawInfo*>::iterator itTextToDraw;
//	for(itTextToDraw = textToDraw.begin(); itTextToDraw != textToDraw.end(); ++itTextToDraw)
//		delete (*itTextToDraw);
//}


#if 1
ElapsedTimer::ElapsedTimer()
	: elapsedTime(0)
	, enableFlag(true)
	, run(false)
{
#if defined(__APPLE__)
	mach_timebase_info(&machTimeInfo);
#endif
}

void ElapsedTimer::enable()
{
	enableFlag = true;
}

void ElapsedTimer::disable()
{
	pause();
	enableFlag = false;
}

void ElapsedTimer::start()
{
	if (!enableFlag)
		return;
	if (!run)
	{
#if defined(_WIN32)
		startInit = timeGetTime(); // svk The timeGetTime function retrieves the system time, in milliseconds. The system time is the time elapsed since Windows was started.
#elif defined(__APPLE__)
		startInit = mach_absolute_time();
#else
		clock_gettime(CLOCK_MONOTONIC, &startInit);
#endif
	}
	run = true;
}

void ElapsedTimer::pause()
{
	if (!run)
		return;
#if defined(_WIN32)
	endInit = timeGetTime();
	elapsedTime += (endInit - startInit) * 1e6;
#elif defined(__APPLE__)
	endInit = mach_absolute_time();
	uint64_t duration = endInit - startInit;
	duration *= machTimeInfo.numer;
	duration /= machTimeInfo.denom;
	elapsedTime += duration;
#else
	clock_gettime(CLOCK_MONOTONIC, &endInit);
	int sec = endInit.tv_sec - startInit.tv_sec;
	if (sec > 0)
		elapsedTime += 1e9 * sec;
	elapsedTime += endInit.tv_nsec - startInit.tv_nsec;
#endif
	run = false;
}

int ElapsedTimer::getElapsedTime()
{
	pause();
	return elapsedTime / 1e6;
}
#endif

#if 0
SkBitmap* RenderingContext::getCachedBitmap(const std::string& bitmapResource) {
	if (defaultIconsDir.size() > 0) {
		string fl = string(defaultIconsDir + "h_" + bitmapResource + ".png");
		FILE* f = fopen(fl.c_str(), "r");
		if (f == NULL) {
			fl = string(defaultIconsDir + "g_" + bitmapResource + ".png");
			f = fopen(fl.c_str(), "r");
		}
		if (f != NULL) {
			fclose(f);
			//flcnav_log_print(LOG_INFO, "Open file %s", fl.c_str());
			SkBitmap* bmp = new SkBitmap();
			if (!SkImageDecoder::DecodeFile(fl.c_str(), bmp)) {
				return NULL;
			}
			return bmp;
		}
	}
	return NULL;
}


unordered_map<std::string, SkBitmap*> cachedBitmaps;
SkBitmap* getCachedBitmap(RenderingContext* rc, const std::string& bitmapResource)
{

	if(bitmapResource.size() == 0)
		return NULL;

	// Try to find previously cached
	unordered_map<std::string, SkBitmap*>::iterator itPreviouslyCachedBitmap = cachedBitmaps.find(bitmapResource);
	if (itPreviouslyCachedBitmap != cachedBitmaps.end())
		return itPreviouslyCachedBitmap->second;
	
	rc->nativeOperations.pause();
	SkBitmap* iconBitmap = rc->getCachedBitmap(bitmapResource);
	cachedBitmaps[bitmapResource] = iconBitmap;
	rc->nativeOperations.start();

	return iconBitmap;
}

void purgeCachedBitmaps() {
	unordered_map<std::string, SkBitmap*>::iterator it = cachedBitmaps.begin();
	for (; it != cachedBitmaps.end(); it++) {
		delete it->second;
	}
}
#endif




inline Sk_float getPowZoom(float zoom){
	if(zoom >= 0 && zoom - floorf(zoom) < 0.05f){
		return 1 << ((int)zoom);
	} else {
		return powf(2, zoom);
	}
}


Sk_float checkLongitude(Sk_float longitude) {
	/*while (longitude < -180 || longitude > 180) {
		if (longitude < 0) {
			longitude += 360;
		} else {
			longitude -= 360;
		}
	}*/
	return longitude;
}

Sk_float checkLatitude(Sk_float latitude) {
	/*while (latitude < -90 || latitude > 90) {
		if (latitude < 0) {
			latitude += 180;
		} else {
			latitude -= 180;
		}
	}
	if (latitude < -85.0511) {
		return -85.0511;
	} else if (latitude > 85.0511) {
		return 85.0511;
	}*/
	return latitude;
}


int get31TileNumberX(Sk_float longitude){
	longitude = checkLongitude(longitude);
	long long int l =  1;
	l <<= 31;
	return (int)((longitude + 180)/360 * l);
}

int get31TileNumberY(Sk_float latitude) {
	latitude = checkLatitude(latitude);
	Sk_float eval = logf(tanf(toRadians(latitude)) + 1 / cosf(toRadians(latitude)));
	long long int l =  1;
	l <<= 31;
	if (eval > M_PI) {
		eval = M_PI;
	}
	return (int) ((1 - eval / M_PI) / 2 * l);
}

Sk_float getLongitudeFromTile(float zoom, Sk_float x) {
	return x / getPowZoom(zoom) * 360.0 - 180.0;
}


Sk_float getLatitudeFromTile(float zoom, Sk_float y){
	int sign = y < 0 ? -1 : 1;
	double result = atanf(sign*sinhf(M_PI * (1 - 2 * y / getPowZoom(zoom)))) * 180. / M_PI;
	return result;
}

Sk_float get31LongitudeX(int tileX){
	return getLongitudeFromTile(21, tileX /1024.);
}

Sk_float get31LatitudeY(int tileY){
	return getLatitudeFromTile(21, tileY /1024.);

}


Sk_float getTileNumberX(float zoom, Sk_float longitude) {
	if (longitude == 180.) {
		return getPowZoom(zoom) - 1;
	}
	longitude = checkLongitude(longitude);
	return (longitude + 180.) / 360. * getPowZoom(zoom);
}

Sk_float convert31YToMeters(int y1, int y2) {
	// translate into meters
	return (y1 - y2) * 0.01863f;
}

Sk_float convert31XToMeters(int x1, int x2) {
	// translate into meters
	return (x1 - x2) * 0.011f;
}

Sk_float calculateProjection31TileMetric(int xA, int yA, int xB, int yB, int xC, int yC) {
	// Scalar multiplication between (AB, AC)
	Sk_float multiple = convert31XToMeters(xB, xA) * convert31XToMeters(xC, xA) + convert31YToMeters(yB, yA) * convert31YToMeters(yC, yA);
	return multiple;
}
Sk_float squareDist31TileMetric(int x1, int y1, int x2, int y2) {
// translate into meters
	Sk_float dy = convert31YToMeters(y1, y2);
	Sk_float dx = convert31XToMeters(x1, x2);
	return dx * dx + dy * dy;
}


//svk
#if !ANDROID
int isnan(double x) { return x != x; } 
int isinf(double x) { return !isnan(x) && isnan(x - x); } 
#endif

Sk_float getTileNumberY(float zoom, Sk_float latitude) {
	latitude = checkLatitude(latitude);
	Sk_float eval = logf(tanf(toRadians(latitude)) + 1 / cosf(toRadians(latitude)));
	if (isinf(eval) || isnan(eval)) {
		latitude = latitude < 0 ? -89.9 : 89.9;
		eval = logf(tanf(toRadians(latitude)) + 1 / cosf(toRadians(latitude)));
	}
	Sk_float result = (1 - eval / M_PI) / 2 * getPowZoom(zoom);
	return result;
}

//Log file impl
void sk_log (int level, const char *source, int line, const char *format, ...)
{
#if 1
	va_list ap;

	 va_start(ap, format);
     char msg[256];
	 vsprintf(msg, format, ap);
    
#if SK_BUILD_FOR_ANDROID      
	  //LOGI(str);
	 LOGI("%s line=%d %s",source, line, msg);
#else
	 char str[256];
	 sprintf(str,"%s line=%d %s",source, line, msg);
	  OutputDebugString(str);
#endif
	 va_end(ap);
#endif
	
}
#if  defined(WIN32) 
void _sk_log ( const char *format, ...)
{

	va_list ap;

	 va_start(ap, format);


	 char str[256];
      char msg[256];

	 vsprintf(msg, format, ap);
      /*sprintf (str, "%c%s %s, line %d %s",
         'N', "DBG", source, line, msg);*/
	 sprintf (str, "%s\r\n", msg);

	  OutputDebugString(str);

	 va_end(ap);

	
}
#endif
void sk_check_allocated_with_source_line
                (const char *source, int line, const void *allocated) {

    if (allocated == NULL) {
        sk_log (SK_MESSAGE_FATAL, source, line, "no more memory");
    }
}