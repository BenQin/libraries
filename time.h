
#ifndef __TIME_H_
#define __TIME_H_
#include "stdbool.h"
#include "stdint.h"

#include "app_timer.h"

/* seconds per day */
#define SPD				(24*60*60UL)

#define RTC_TICKS          APP_TIMER_TICKS(1000, 0)

#define SUN			(0)
#define MON			(1)
#define TUE			(2)
#define WED			(3)
#define THU			(4)
#define FRI			(5)
#define SAT			(6)

typedef uint32_t time_t;

/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};

/*
 * Structure defined by POSIX.1b to be like a timeval.
 */
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* and nanoseconds */
};

struct timezone {
  int tz_minuteswest;	/* minutes west of Greenwich */
  int tz_dsttime;	/* type of dst correction */
};



#if 0
struct tm {
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year - 1900. */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};

#else
struct tm {
  int8_t tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int8_t tm_min;			/* Minutes.	[0-59] */
  int8_t tm_hour;			/* Hours.	[0-23] */
  int8_t tm_mday;			/* Day.		[1-31] */
  int8_t tm_mon;			/* Month.	[0-11] */
  int16_t tm_year;			/* Year - 1900. */
  int8_t tm_wday;			/* Day of week.	[0-6] */
  int16_t tm_yday;			/* Days in year.[0-365]	*/
  int8_t tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};
#endif


typedef enum
{
	TIME_EVT_BEGIN = 0,

	TIME_EVT_NEW_SEC,
	TIME_EVT_NEW_MIN,
	TIME_EVT_NEW_HOUR,
	TIME_EVT_NEW_DAY,

	TIME_EVT_END
} time_evt_t;


void rtc_srv_init(void);
struct tm *gmtime_r(const time_t *p_timestamp, struct tm *r);
struct tm* localtime(const time_t* t);
time_t time(time_t *t);
struct tm* localtime_r(const time_t* t, struct tm* r);
int gettimeofday(struct timeval *p_timeval, struct timezone *p_timezone);
time_t mktime(struct tm * const t);
void set_date(uint32_t year, uint32_t month, uint32_t day);
void set_time(uint32_t hour, uint32_t minute, uint32_t second);
void set_date_time(int16_t year, int8_t mon, int8_t day, int8_t hour, int8_t min, int8_t sec);
void set_unix_date_time(time_t utime);
bool set_timezone(int timezone, int dsttime);

#endif


