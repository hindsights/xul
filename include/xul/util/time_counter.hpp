#pragma once

#include <xul/util/ptime_counter.hpp>
#include <xul/std/strings.hpp>
#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <xul/config.hpp>
#include <stdint.h>
#include <assert.h>

#ifdef XUL_MACH
#include <mach/clock.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach_port.h>
#endif

namespace xul {

namespace detail {

template <typename ImplT>
class simple_time_counter
{
public:
    typedef ImplT impl_type;
    typedef typename ImplT::count_value_type count_value_type;

    BOOST_STATIC_ASSERT(sizeof(count_value_type) % sizeof(int) == 0);

    static count_value_type get_system_count()
    {
        return ImplT::get_current_count();
    }

    static count_value_type get_realtime_count()
    {
        return get_system_count();
    }

    static uint32_t get_system_count32()
    {
        return static_cast<uint32_t>(get_system_count());
    }

    simple_time_counter()
    {
        m_count = get_realtime_count();
    }
    explicit simple_time_counter(count_value_type count) : m_count(count)
    {
    }

    void sync()
    {
        m_count = get_realtime_count();
    }

    count_value_type elapsed() const
    {
        return get_realtime_count() - m_count;
    }
    count_value_type get_elapsed() const
    {
        return get_realtime_count() - m_count;
    }

    boost::posix_time::time_duration elapsed_time() const
    {
        return boost::posix_time::milliseconds(get_elapsed());
    }
    boost::posix_time::time_duration get_elapsed_time() const
    {
        return boost::posix_time::milliseconds(get_elapsed());
    }
    boost::posix_time::ptime get_local_time() const
    {
        return local_ptime_counter::get_system_time() - elapsed_time();
    }

    boost::posix_time::ptime get_utc_time() const
    {
        return ptime_counter::get_system_time() - elapsed_time();
    }


    uint32_t get_elapsed32() const
    {
        return static_cast<uint32_t>(get_elapsed());
    }

    uint32_t elapsed32() const
    {
        return get_elapsed32();
    }

    count_value_type get_count() const
    {
        return m_count;
    }
    operator count_value_type() const
    {
        return get_count();
    }

private:
    count_value_type m_count;
};

}
}


#if defined(BOOST_WINDOWS)

#include <xul/mswin/windows.hpp>

namespace xul {
namespace detail {

class tick_counter64
{
public:
    typedef int64_t count_value_type;
    tick_counter64()
    {
        m_count32 = ::GetTickCount();
        m_count64 = m_count32;
    }

    int64_t get_count()
    {
        DWORD newCount32 = ::GetTickCount();
        DWORD diff = newCount32 - m_count32;
        m_count32 = newCount32;
        m_count64 += diff;
        return m_count64;
    }

    static int64_t get_current_count()
    {
        static tick_counter64 the_counter;
        return the_counter.get_count();
    }

private:
    int64_t m_count64;
    DWORD m_count32;
};

inline uint32_t GetTickCount()
{
    return ::GetTickCount();
}

class tick_counter32
{
public:
    typedef DWORD count_value_type;
    static DWORD get_current_count()
    {
        return ::GetTickCount();
    }
};


/// high-precision time counter using QueryPerformanceCounter/QueryPerformanceFrequency
class accurate_tick_counter
{
public:
    typedef int64_t count_value_type;

    static int64_t get_frequency()
    {
        static int64_t the_frequency = query_frequency();
        return the_frequency;
    }
    static int64_t get_current_count()
    {
        int64_t freq = get_frequency();
        if (freq <= 0)
            return ::GetTickCount();
        int64_t counter = query_counter();
        if (counter <= 0)
            return ::GetTickCount();
        /// avoid overflow
        return counter * 1000.0 / freq;
    }

private:
    static int64_t query_frequency()
    {
        LARGE_INTEGER val = { 0 };
        if (::QueryPerformanceFrequency(&val))
            return val.QuadPart;
        return 0;
    }
    static int64_t query_counter()
    {
        LARGE_INTEGER val = { 0 };
        if (::QueryPerformanceCounter(&val))
            return val.QuadPart;
        return 0;
    }
};

}


/// time counter base on GetTickCount(unit: millisecond)
class coarse_time_counter
{
public:
    typedef int64_t count_value_type;

    static int64_t get_system_count()
    {
        static detail::tick_counter64 tickCounter;
        return tickCounter.get_count();
    }

    static DWORD get_system_count32()
    {
        return ::GetTickCount();
    }

    coarse_time_counter()
    {
        m_count = get_realtime_count();
    }
    explicit coarse_time_counter(int64_t count) : m_count(count)
    {
    }

    /// synchronize to current time
    void sync()
    {
        m_count = get_realtime_count();
    }
    /// calculate elapsed time from last synchronization
    int64_t elapsed() const
    {
        return get_realtime_count() - m_count;
    }
    int64_t get_elapsed() const
    {
        return get_realtime_count() - m_count;
    }
    /// calculate elapsed time from last synchronization(32 bits)
    DWORD elapsed32() const
    {
        return static_cast<DWORD>( get_realtime_count() - m_count );
    }
    DWORD get_elapsed32() const
    {
        return static_cast<DWORD>( get_realtime_count() - m_count );
    }

    /// calculate elapsed time_duration from last synchronization
    boost::posix_time::time_duration elapsed_time() const
    {
        return get_elapsed_time();
    }
    boost::posix_time::time_duration get_elapsed_time() const
    {
        return boost::posix_time::milliseconds(elapsed());
    }

    /// calculate ptime of last synchronization
    boost::posix_time::ptime get_local_time() const
    {
        return local_ptime_counter::get_system_time() - elapsed_time();
    }

    boost::posix_time::ptime get_utc_time() const
    {
        return ptime_counter::get_system_time() - elapsed_time();
    }

    int64_t get_count() const
    {
        return m_count;
    }
    operator int64_t() const
    {
        return get_count();
    }

    int64_t get_realtime_count() const
    {
        return m_tick.get_count();
    }

private:
    int64_t m_count;
    mutable detail::tick_counter64 m_tick;
};

typedef detail::simple_time_counter<detail::tick_counter32> time_counter32;
typedef detail::simple_time_counter<detail::accurate_tick_counter> accurate_time_counter;
typedef accurate_time_counter time_counter;
typedef accurate_time_counter safe_time_counter;

}

#elif defined(BOOST_HAS_CLOCK_GETTIME) || defined(XUL_MACH)


#include <sys/time.h>
#include <time.h>


namespace xul {

namespace detail {

inline int64_t GetTickCount64()
{
    struct timespec t = { 0 };
#if defined(XUL_MACH)
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t.tv_sec = mts.tv_sec;
    t.tv_nsec = mts.tv_nsec;
#else
#ifdef CLOCK_MONOTONIC_RAW
    int res = clock_gettime(CLOCK_MONOTONIC_RAW, &t);
#else
    int res = clock_gettime(CLOCK_MONOTONIC, &t);
#endif
    assert( 0 == res );
#endif
    double val = t.tv_sec * 1000.0 + t.tv_nsec * 0.000001;
    return static_cast<int64_t>( val );
}

inline uint32_t GetTickCount()
{
    int64_t ticks = detail::GetTickCount64();
    return static_cast<uint32_t>( ticks );
}

class tick_counter64
{
public:
    typedef int64_t count_value_type;
    static int64_t get_current_count()
    {
        return GetTickCount64();
    }
};

class tick_counter32
{
public:
    typedef uint32_t count_value_type;
    static uint32_t get_current_count()
    {
        return GetTickCount();
    }
};

}

typedef detail::simple_time_counter<detail::tick_counter32> time_counter32;
typedef detail::simple_time_counter<detail::tick_counter64> time_counter;
typedef time_counter accurate_time_counter;

}

#if defined(ANDROID)
#include <sys/time.h>
#include <linux/ioctl.h>
#include <linux/rtc.h>
#include <linux/android_alarm.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
namespace xul {
namespace detail {

inline int64_t doGetElapsedRealtime()
{
    struct timespec ts;
    int fd, result;

    fd = open("/dev/alarm", O_RDONLY);
    if (fd < 0)
        return fd;

   result = ioctl(fd, ANDROID_ALARM_GET_TIME(ANDROID_ALARM_ELAPSED_REALTIME), &ts);
   close(fd);

    if (result == 0)
        return static_cast<int64_t>(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
    return -1;
}

inline int64_t SafeGetTickCount64()
{
    int64_t t = doGetElapsedRealtime();
    if (t <= 0)
        return GetTickCount64();
    return t;
}

inline uint32_t SafeGetTickCount()
{
    int64_t ticks = detail::SafeGetTickCount64();
    return static_cast<uint32_t>( ticks );
}

class safe_tick_counter64
{
public:
    typedef int64_t count_value_type;
    static int64_t get_current_count()
    {
        return SafeGetTickCount64();
    }
};

class safe_tick_counter32
{
public:
    typedef uint32_t count_value_type;
    static uint32_t get_current_count()
    {
        return SafeGetTickCount();
    }
};

typedef detail::simple_time_counter<detail::safe_tick_counter32> safe_time_counter32;
typedef detail::simple_time_counter<detail::safe_tick_counter64> safe_time_counter;
}
}
#else
namespace xul {
typedef time_counter32 safe_time_counter32;
typedef time_counter safe_time_counter;
}
#endif



#else

#error unsupported platform for TimeCounter

#endif


namespace xul {

class time_span
{
public:
    enum
    {
        TICKS_PER_SECOND = 1000,
        TICKS_PER_MINUTE = 1000 * 60,
        TICKS_PER_HOUR = 1000 * 60 * 60,
        TICKS_PER_DAY = 1000 * 60 * 60 * 24,
    };

    explicit time_span( int64_t diff = 0 ) : m_diff( diff ) { }

    int64_t total_ticks() const { return m_diff; }
    int64_t total_days() const { return m_diff / TICKS_PER_DAY; }
    int64_t total_hours() const { return m_diff / TICKS_PER_HOUR; }
    int64_t total_minutes() const { return m_diff / TICKS_PER_MINUTE; }
    int64_t total_seconds() const { return m_diff / TICKS_PER_SECOND; }
    int64_t total_milliseconds() const { return m_diff; }

    uint32_t days() const { return static_cast<uint32_t>( this->total_days() ); }

    uint32_t hours() const
    {
        uint32_t oddTicks = static_cast<uint32_t>( m_diff % TICKS_PER_DAY );
        return oddTicks / TICKS_PER_HOUR;
    }
    uint32_t minutes() const
    {
        uint32_t oddTicks = static_cast<uint32_t>( m_diff % TICKS_PER_HOUR );
        return oddTicks / TICKS_PER_MINUTE;
    }
    uint32_t seconds() const
    {
        uint32_t oddTicks = static_cast<uint32_t>( m_diff % TICKS_PER_MINUTE );
        return oddTicks / TICKS_PER_SECOND;
    }
    uint32_t milliseconds() const
    {
        uint32_t oddTicks = static_cast<uint32_t>( m_diff % TICKS_PER_SECOND );
        return oddTicks;
    }

    std::string str() const
    {
        return xul::strings::format( "%02u-%02u:%02u:%02u.%03u", days(), hours(), minutes(), seconds(), milliseconds() );
    }

private:
    int64_t m_diff;
};

}

