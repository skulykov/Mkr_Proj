#ifndef INCLUDE__MKRNAV__H
#define INCLUDE__MKRNAV__H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if SK_BUILD_FOR_ANDROID
#include <sys/time.h>
#else
#include <Windows.h>
#include <WinSock.h>
#define snprintf _snprintf 
#endif

#include <time.h>
#include "dbg.h"
#ifdef __SYMBIAN32__
typedef   unsigned int      uint32_t;
typedef unsigned short      uint16_t;
#elif defined MKR_WIN32

   #define INVALID_THREAD_ID        (0xFFFFFFFF)

   #include "Win32\stdint.h"

   #if(!(defined WINCE) && !(defined _WIN32_WCE))
      #define WIN32PC
      ///[BOOKMARK]:[NOTE] - Here you can define Touch-Screen build for Windows PC (not device)
      #define TOUCH_SCREEN
   #endif   // PC only

   #if(defined _DEBUG || defined DEBUG)
      #define  WIN32_DEBUG
   #endif   // DEBUG

#else

   #include <stdint.h>

#endif

#if SK_BUILD_FOR_ANDROID
typedef bool BOOL;
#endif
typedef unsigned int size_t;

#ifndef TRUE
#define TRUE		true
#endif
#ifndef FALSE
#define FALSE		false
#endif
#ifndef NULL
#define NULL		0L
#endif


#define  CLIENT_VERSION_MAJOR       (2)
#define  CLIENT_VERSION_MINOR       (4)
#define  CLIENT_VERSION_SUB         (0)
#define  CLIENT_VERSION_CFG         (26)	

typedef void (* Sk_Callback) (void);

typedef enum tag_sk_result
{
   // Success:
   succeeded   = 0,
   no_error    = 0,

   // General errors
   general_errors,
      err_failed,
      err_no_memory,
      err_invalid_argument,
      err_aborted,
      err_access_denied,
      err_timed_out,
      err_internal_error,
   general_errors_end,

   // Network errors
   network_errors,
      err_net_failed,
      err_net_unknown_protocol,
      err_net_remote_error,
      err_net_request_pending,
      err_net_no_path_to_destination,
   network_errors_end,

   // Data parser errors
   parser_errors,
      err_parser_unexpected_data,
      err_parser_failed,
      err_parser_missing_tag_handler,
   parser_errors_end,

   // Realtime errors
   realtime_errors,
      err_rt_no_data_to_send,
      err_rt_login_failed,
      err_rt_wrong_name_or_password,
      err_rt_wrong_network_settings,
      err_rt_unknown_login_id,
   realtime_errors_end,

   // Update account errors
   update_account_errors,
      err_upd_account_invalid_user_name,
      err_upd_account_name_already_exists,
      err_upd_account_invalid_password,
      err_upd_account_invalid_email,
      err_upd_account_email_exists,
      err_upd_account_cannot_complete_request,
   update_account_errors_end,

   // Address Search - String Resolution
   address_search,
      err_as_could_not_find_matches,
      err_as_wrong_input_string_size,
      err_as_already_in_transaction,
   address_search_end,

   // This must be last:
   all_errors_end,
   invalid_error

}  sk_result;


#define SAFE_STR( str ) ( ( str ) ? ( str ) : "NULL" )

class Mkr
{
public:
	static char *sk_lang_get( const char * str)
	{
		return (char*) &str[0];
	}
	static const char* sk_start_version() {
	   static char version[64];
	   static int done = 0;

	   if( done)
		  return version;

	   sprintf( version,
				"%d.%d.%d.%d",
				CLIENT_VERSION_MAJOR,
				CLIENT_VERSION_MINOR,
				CLIENT_VERSION_SUB,
				CLIENT_VERSION_CFG );

	   done = TRUE;
	   return version;
	}
};


typedef struct
{
   time_t epoch_sec;
   unsigned long usec; // Micro second fraction
} EpochTimeMicroSec;

#if !SK_BUILD_FOR_ANDROID
// for windows
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
 
struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
 
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tmpres /= 10;  /*convert into microseconds*/
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}
#endif
class Sk_CTime
{
	static uint32_t tv_to_msec(struct timeval *tv)
	{
	   //return (tv->tv_sec & 0xffff) * 1000 + tv->tv_usec/1000;
   
	   static time_t initial_sec = 0;
   
	   if (initial_sec == 0) {
		  initial_sec = tv->tv_sec;
	   }
   
	   return (uint32_t)(tv->tv_sec - initial_sec) * 1000 + (uint32_t)tv->tv_usec/1000;
	}
public:
	static char *sk_time_get_hours_minutes (time_t gmt) {
    
		static char image[16];
    
		struct tm *tm;
    
		tm = localtime ((const time_t *)&gmt);
		snprintf (image, sizeof(image), "%2d:%02d", tm->tm_hour, tm->tm_min);

		return image;
	}

	

	static uint32_t sk_time_get_millis(void) {
	   struct timeval tv;

	   gettimeofday(&tv, NULL);
	   return tv_to_msec(&tv);
	}

	static const EpochTimeMicroSec* sk_time_get_epoch_us( EpochTimeMicroSec* time_val )
	{
	   static EpochTimeMicroSec s_epoch = { 0, 0 };
	   struct timeval tv;
	   gettimeofday(&tv, NULL);

	   s_epoch.epoch_sec = tv.tv_sec;
	   s_epoch.usec = tv.tv_usec;

	   if ( time_val )
		  *time_val = s_epoch;

	   return &s_epoch;
	}

};

#endif //INCLUDE__MKRNAV__H