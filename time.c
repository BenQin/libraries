#include "math.h"
#include "nrf_soc.h"
#include "ble_gap.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_trace.h"
#include "motion.h"
#include "uv.h"
#include "backup.h"
#include "time.h"

#define SYS_CLOCK_DEFAULT_YEAR		(2015)
#define SYS_CLOCK_DEFAULT_MONTH		(4)
#define SYS_CLOCK_DEFAULT_DAY		(1)
#define SYS_CLOCK_DEFAULT_HOUR		(11)
#define SYS_CLOCK_DEFAULT_MIN		(59)
#define SYS_CLOCK_DEFAULT_SEC		(29)

/* days per month -- nonleap! */
static const short m_sum_days_of_months[13] =
{ 
	0,
	(31),
	(31+28),
	(31+28+31),
	(31+28+31+30),
	(31+28+31+30+31),
	(31+28+31+30+31+30),
	(31+28+31+30+31+30+31),
	(31+28+31+30+31+30+31+31),
	(31+28+31+30+31+30+31+31+30),
	(31+28+31+30+31+30+31+31+30+31),
	(31+28+31+30+31+30+31+31+30+31+30),
	(31+28+31+30+31+30+31+31+30+31+30+31),
};

static const char days[] = "SUN MON TUE WED THU FRI SAT ";
static const char months[] = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";


static app_timer_id_t m_rtc_timer_id;	
time_t m_timestamp = 1427889569;
static struct tm m_time;
static struct timezone m_timezone;


static void (* const m_time_evt_handler_tab[])(const void *p_content, uint32_t size) = 
{
	motion_time_evt_handler,
	sleep_time_evt_handler,
	uv_time_evt_handler,
	backup_time_evt_handler,
};


static void time_evt_dispatch(const void *p_content, uint32_t size)
{
	for (uint32_t i = 0; i < sizeof(m_time_evt_handler_tab) / sizeof(m_time_evt_handler_tab[0]); ++i)
	{
		m_time_evt_handler_tab[i](p_content, size);
	}
}


void tick_callback(void * p_event_data, uint16_t event_size)
{
	static time_evt_t time_evt[4];
	uint32_t i = 0;

	time_evt[i++] = TIME_EVT_NEW_SEC;
	if (m_timestamp % 60 == 0)
	{
		time_evt[i++] = TIME_EVT_NEW_MIN;
		if (m_timestamp % 3600 == 0)
		{
			time_evt[i++] = TIME_EVT_NEW_HOUR;
			if (m_timestamp % SPD == 0)
			{
				time_evt[i++] = TIME_EVT_NEW_DAY;
			}
		}
	}
	//uv_abserver();
	//motion_abserver();
	app_trace_log("timestamp %d\r\n", m_timestamp);
	time_evt_dispatch(time_evt, i * sizeof(time_evt_t));
}


static void rtc_callback(void * p_context)
{
	++m_timestamp;
	app_sched_event_put(NULL, 0, tick_callback);
}


static void rtc_init(void)
{
	uint32_t err_code;
    err_code = app_timer_create(&m_rtc_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                rtc_callback);
    APP_ERROR_CHECK(err_code);
}


static void rtc_start(void)
{
	uint32_t err_code;

	err_code = app_timer_start(m_rtc_timer_id, RTC_TICKS, NULL);
	APP_ERROR_CHECK(err_code);
}


static void rtc_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(m_rtc_timer_id);
	APP_ERROR_CHECK(err_code);
}


void rtc_srv_init(void)
{
	rtc_init();
	//set_date_time(SYS_CLOCK_DEFAULT_YEAR, SYS_CLOCK_DEFAULT_MONTH, SYS_CLOCK_DEFAULT_DAY,
		//			SYS_CLOCK_DEFAULT_HOUR, SYS_CLOCK_DEFAULT_MIN, SYS_CLOCK_DEFAULT_SEC);
	set_timezone(8, 0);
	rtc_start();
}


time_t time(time_t *t)
{
    if (t != NULL)
    {
        *t = m_timestamp;
    }

    return m_timestamp;
}


int leap_year(int year)
{
	/* every fourth year is a leap year except for century years that are
	 * not divisible by 400. */
	/*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}


struct tm *gmtime_r(const time_t *p_timestamp, struct tm *r)
{
	time_t i;
	register time_t work;
	
	work = *p_timestamp % SPD;
	r->tm_sec = work % 60;
	work /= 60;
	r->tm_min = work % 60;
	r->tm_hour = work / 60;
	work = *p_timestamp / SPD;
	r->tm_wday = (THU + work) % 7;
	for (i = 1970; ; ++i)
	{
		register time_t k = leap_year(i) ? 366 : 365;
		if (work >= k)
			work -= k;
		else
			break;
	}
	r->tm_year = i - 1900;
	r->tm_yday = work;

	r->tm_mday = 1;
	if (leap_year(i) && (work > 58))
	{
		if (work == 59)
			r->tm_mday = 2; /* 29.2. */
		work -= 1;
	}

	for (i = 11; i && (m_sum_days_of_months[i] > work); --i)
		;
	r->tm_mon = i;
	r->tm_mday += work - m_sum_days_of_months[i];
	
	return r;
}


struct tm* localtime_r(const time_t* t, struct tm* r)
{
	time_t time;
	struct timeval tv;
	struct timezone tz;
	
	gettimeofday(&tv, &tz);
	time = *t - tz.tz_minuteswest * 60UL;
	
	return gmtime_r(&time, r);
}


struct tm* localtime(const time_t* t)
{
	time_t time;
	struct timeval tv;
	struct timezone tz;
	
	gettimeofday(&tv, &tz);
	time = *t - tz.tz_minuteswest * 60UL;
	
	return gmtime_r(&time, &m_time);
}


time_t mktime(struct tm * const t)
{
	register time_t day;
	register time_t i;
	register time_t years = t->tm_year - (1970 - 1900);//timestamp:from 1970/0/1 0:0:0   year:from 1900

	if (t->tm_sec >= 60)
	{
		t->tm_min += t->tm_sec / 60;
		t->tm_sec %= 60;
	}
	if (t->tm_min >= 60)
	{
		t->tm_hour += t->tm_min / 60;
		t->tm_min %= 60;
	}
	if (t->tm_hour >= 24)
	{
		t->tm_mday += t->tm_hour / 24;
		t->tm_hour %= 24;
	}
	if (t->tm_mon >= 12)
	{
		t->tm_year += t->tm_mon / 12;
		t->tm_mon %= 12;
	}
	while (t->tm_mday > m_sum_days_of_months[1 + t->tm_mon])
	{
		/* leap year, Feb: total days(tm_mday) - 1 */
		if (t->tm_mon == 1 && leap_year(t->tm_year + 1900))
		{
			--t->tm_mday;
		}
		t->tm_mday -= m_sum_days_of_months[t->tm_mon];//sum[1+tm_mon] - sum[1+tm_mon-1]
		++t->tm_mon;
		if (t->tm_mon > 11)
		{
			t->tm_mon = 0;
			++t->tm_year;
		}
	}


	if (t->tm_year < 70)
		return (time_t)-1;

	/* Days since 1970 is 365 * number of years + number of leap years since 1970 */
	day = years * 365 + (years + 1) / 4;//(1973 - 1970 + 1) / 4 = 1 means leap day in 1972

	/* After 2100 we have to substract 3 leap years for every 400 years
	 This is not intuitive. Most mktime implementations do not support
	 dates after 2059, anyway, so we might leave this out for it's
	 bloat. */
	if (years >= 131)
	{
		years -= 131;
		years /= 100;
		day -= (years >> 2) * 3 + 1;
		if ((years &= 3) == 3)
			years--;
		day -= years;
	}
	/* mday:1-31, yday: 0-365.  so, t->tm_mday - 1 */
	day += t->tm_yday = m_sum_days_of_months[t->tm_mon] + t->tm_mday - 1 +
			(leap_year(t->tm_year + 1900) & (t->tm_mon > 1));

	/* day is now the number of days since 'Jan 1 1970' */
	i = 7;
	t->tm_wday = (day + 4) % i; /* Sunday=0, Monday=1, ..., Saturday=6 */

	i = 24;
	day *= i;
	i = 60;
	return ((day + t->tm_hour) * i + t->tm_min) * i + t->tm_sec;
}


static void num2str(char *c, int i)
{
	c[0] = i / 10 + '0';
	c[1] = i % 10 + '0';
}


char * asctime_r(const struct tm *p_time, char *buf)
{
	/* "Wed Jun 30 21:49:08 1993\n" */
	*(int*) buf = *(int*) (days + (p_time->tm_wday << 2));
	*(int*) (buf + 4) = *(int*) (months + (p_time->tm_mon << 2));
	num2str(buf + 8, p_time->tm_mday);
	if (buf[8] == '0')
		buf[8] = ' ';
	buf[10] = ' ';
	num2str(buf + 11, p_time->tm_hour);
	buf[13] = ':';
	num2str(buf + 14, p_time->tm_min);
	buf[16] = ':';
	num2str(buf + 17, p_time->tm_sec);
	buf[19] = ' ';
	num2str(buf + 20, (p_time->tm_year + 1900) / 100);
	num2str(buf + 22, (p_time->tm_year + 1900) % 100);
	buf[24] = '\n';
	
	return buf;
}


char *asctime(const struct tm *p_time)
{
	static char buf[25];
	
	return asctime_r(p_time, buf);
}


char *ctime(const time_t *timep)
{
	return asctime(localtime((time_t*)timep));
}


int gettimeofday(struct timeval *p_timeval, struct timezone *p_timezone)
{
	if(p_timeval != NULL)
	{
		p_timeval->tv_sec = m_timestamp;
		p_timeval->tv_usec = 0;
	}
	if (p_timezone != NULL)
	{
		p_timezone->tz_minuteswest = m_timezone.tz_minuteswest;
		p_timezone->tz_dsttime = m_timezone.tz_dsttime;
	}
	
	return 0;
}


void set_date(uint32_t year, uint32_t month, uint32_t day)
{
    time_t now;
    struct tm* p_time = NULL;

    /* get current time */
    now = m_timestamp;

    p_time = localtime(&now);
    if (p_time != 0)
    {
        p_time->tm_year = year - 1900;
        p_time->tm_mon 	= month - 1; /* p_time->tm_mon = month; 0~11 */
        p_time->tm_mday = day;
    }

    now = mktime(p_time);

    m_timestamp = now;
}


void set_time(uint32_t hour, uint32_t minute, uint32_t second)
{
    time_t now;
    struct tm * p_time = NULL;

    /* get current time */
    now = m_timestamp;

    p_time = localtime(&now);
    if (p_time != NULL)
    {
        p_time->tm_hour = hour;
        p_time->tm_min 	= minute;
        p_time->tm_sec 	= second;
    }

    now = mktime(p_time);
    m_timestamp = now;
}


void set_date_time(int16_t year, int8_t mon, int8_t day, int8_t hour, int8_t min, int8_t sec)
{
	time_t now;
	
	m_time.tm_year = year - 1900;
	m_time.tm_mon = mon - 1; /* ti->tm_mon	= month; 0~11 */
	m_time.tm_mday = day;
	m_time.tm_hour = hour;
	m_time.tm_min = min;
	m_time.tm_sec = sec;

	now = mktime(&m_time);
	m_timestamp = now;
}


void set_unix_date_time(time_t unix_time)
{
	rtc_stop();
	m_timestamp = unix_time + 1;
	rtc_start();
}


bool set_timezone(int timezone, int dsttime)
{
	bool success = false;
	
	if (timezone >= -12 && timezone <= 12)
	{
		m_timezone.tz_minuteswest = -timezone * 60;
		m_timezone.tz_dsttime = dsttime;
		success = true;
	}

	return success;
}
