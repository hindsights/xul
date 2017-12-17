#pragma once

#include <xul/config.hpp>
#include <xul/std/strings.hpp>
#include <xul/macro/fill_zero.hpp>
#include <boost/config.hpp>

#include <sys/timeb.h>

#if defined(BOOST_HAS_FTIME) || defined(BOOST_HAS_GETTIMEOFDAY)
#define XUL_DATE_TIME_USE_BOOST
#endif

#if defined(XUL_WINDOWS)
#  if defined(XUL_DATE_TIME_USE_BOOST)
#    include <boost/date_time/posix_time/ptime.hpp>
#    include <boost/date_time/posix_time/posix_time_types.hpp>
#  endif
#  include <xul/mswin/windows.hpp>
#endif

#include <time.h>
#include <stdint.h>


namespace xul {


const uint64_t nanoseconds_100_per_day = 24ULL * 60 * 60 * 1000 * 1000 * 10;


/// Class to provide simple basic formatting rules
template<class charT>
class simple_date_time_format {
public:
    /// char between year-month
    static charT month_sep_char()
    {
        return '-';
    }
    /// Char between month-day
    static charT day_sep_char()
    {
        return '-';
    }
    /// char between date-hour
    static charT hour_sep_char()
    {
        return ' ';
    }
    /// char between hour and minute
    static charT minute_sep_char()
    {
        return ':';
    }
    /// char for second
    static charT second_sep_char()
    {
        return ':';
    }
    /// char for second
    static charT millisecond_sep_char()
    {
        return '.';
    }

};

#if defined(XUL_WINDOWS)

#if defined(XUL_DATE_TIME_USE_BOOST)

class date_time
{
public:
    date_time()
    {
    }

    int year() const
    {
        return m_time.date().year();
    }
    int month() const
    {
        return m_time.date().month();
    }
    int day() const
    {
        return m_time.date().day();
    }
    int hour() const
    {
        return m_time.time_of_day().hours();
    }
    int minute() const
    {
        return m_time.time_of_day().minutes();
    }
    int second() const
    {
        return m_time.time_of_day().seconds();
    }
    int millisecond() const
    {
        boost::posix_time::time_duration td = m_time.time_of_day();
        if (td.ticks_per_second() < 1000)
            return td.ticks() * 1000 / td.ticks_per_second();
        return td.ticks() % td.ticks_per_second() / 1000;
    }

    std::string str() const
    {
        return format( "%04u-%02u-%02u %02u:%02u:%02u.%03u" );
    }

    std::string str_no_milliseconds() const
    {
        return strings::format(
            "%04u-%02u-%02u %02u:%02u:%02u.%03u",
            year(),
            month(),
            day(),
            hour(),
            minute(),
            second());
    }

    std::string format( const char* formatStr ) const
    {
        return strings::format(
            formatStr,
            year(),
            month(),
            day(),
            hour(),
            minute(),
            second(),
            millisecond() );
    }

    static date_time now()
    {
        date_time dt;
        dt.m_time = boost::posix_time::microsec_clock::local_time();
        return dt;
    }

    static date_time utc_now()
    {
        date_time dt;
        dt.m_time = boost::posix_time::microsec_clock::universal_time();
        return dt;
    }

private:
    boost::posix_time::ptime m_time;
};

#else

class date_time
{
public:
    date_time()
    {
        XUL_FILL_ZERO( m_time );
    }

    int year() const
    {
        return m_time.wYear;
    }
    int month() const
    {
        return m_time.wMonth;
    }
    int day() const
    {
        return m_time.wDay;
    }
    int hour() const
    {
        return m_time.wHour;
    }
    int minute() const
    {
        return m_time.wMinute;
    }
    int second() const
    {
        return m_time.wSecond;
    }
    int millisecond() const
    {
        return m_time.wMilliseconds;
    }

    std::string str() const
    {
        return format( "%04u-%02u-%02u %02u:%02u:%02u.%03u" );
    }

    std::string str_no_milliseconds() const
    {
        return strings::format(
            "%04u-%02u-%02u %02u:%02u:%02u.%03u",
            year(),
            month(),
            day(),
            hour(),
            minute(),
            second());
    }

    std::string format( const char* formatStr ) const
    {
        return strings::format(
            formatStr,
            year(),
            month(),
            day(),
            hour(),
            minute(),
            second(),
            millisecond() );
    }

    static date_time now()
    {
        date_time dt;
        ::GetLocalTime( &dt.m_time );
        return dt;
    }

    static date_time utc_now()
    {
        date_time dt;
        ::GetSystemTime( &dt.m_time );
        return dt;
    }

private:
    SYSTEMTIME m_time;
};

#endif

#else

class date_time
{
public:
    date_time()
    {
        XUL_FILL_ZERO( m_date );
        XUL_FILL_ZERO( m_time );
    }

    int year() const
    {
        return m_date.tm_year + 1900;
    }
    int month() const
    {
        return m_date.tm_mon + 1;
    }
    int day() const
    {
        return m_date.tm_mday;
    }
    int hour() const
    {
        return m_date.tm_hour;
    }
    int minute() const
    {
        return m_date.tm_min;
    }
    int second() const
    {
        return m_date.tm_sec;
    }
    int millisecond() const
    {
        return m_time.millitm;
    }

    std::string str() const
    {
        return format( "%04u-%02u-%02u %02u:%02u:%02u.%03u" );
    }

    std::string str_no_milliseconds() const
    {
        return strings::format(
            "%04u-%02u-%02u %02u:%02u:%02u.%03u",
            year(),
            month(),
            day(),
            hour(),
            minute(),
            second());
    }

    std::string format( const char* formatStr ) const
    {
        return strings::format(
            formatStr,
            year(),
            month(),
            day(),
            hour(),
            minute(),
            second(),
            millisecond() );
    }

    static date_time now()
    {
        date_time dt;
        ftime( &dt.m_time );
        tm* t = localtime( &dt.m_time.time );
        if ( t != NULL )
        {
            dt.m_date = *t;
        }
        return dt;
    }

    static date_time utc_now()
    {
        date_time dt;
        ftime( &dt.m_time );
        tm* t = gmtime( &dt.m_time.time );
        if ( t != NULL )
        {
            dt.m_date = *t;
        }
        return dt;
    }

private:
    struct tm m_date;
    struct timeb m_time;
};

#endif


}
