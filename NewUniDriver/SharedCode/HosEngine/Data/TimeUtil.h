#ifndef TimeUtil_H
#define TimeUtil_H

#include <string>
#include <cmath>
#include "my_time.h"
#define USE_SIMPLE_MIN

namespace HosEngine
{
	class TimeUtil final
	{
	private:
		static float TimeZoneFromServer;
		static int ApplyDayTimeZone;
		static int DLSaving;
	public:
		static void SetTimeZoneFromServer(float v, int applydts);
		static DateTime DateTime_Now();

		static DateTime CurrentTime() ;
		static DateTime SkipMs(DateTime d);
		#if defined(USE_SIMPLE_MIN)
		static DateTime roundMinute(DateTime d);
		#else

		static int RULE_LOGTIME_INTERVAL;
		static int DataSetSaveInterval; //30sec
		static int DataSetUpdateInterval; //30 secondnd

		static DateTime roundMinute(DateTime d);
		#endif

		static int getDayLightSaving();

		static std::string LogTimeToString(DateTime logtime);
		static DateTime ParseLogTime(const std::string &logtime);
		static DateTime ParseDateTime(const std::string &str, const std::string &format);

		static std::string ConvertMinutesToHoursandMinutes(int totalMinues);
		static std::string GetTimeStr(DateTime dt);
		static std::string GetLongTimeStr(DateTime dt);
		static std::string GetLongTimeStr_1(DateTime dt);
		static int getTimeZoneOffset(bool flag = false);


	};
}

#endif