#pragma once

//#include <mmsystem.h>
//#pragma comment(lib, "winmm.lib")

#include <xul/util/time_counter.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <winbase.h>


namespace xul { namespace detail {

class tick_count_timer_traits
{
public:
    class time_traits_type
    {
    public:
        // The duration type.
        class duration_type
        {
        public:
            duration_type() : ticks_(0) {}
            explicit duration_type(DWORD ticks) : ticks_(ticks) {}
        private:
            friend class time_traits_type;
            DWORD ticks_;
        };

        // The time type. This type has no constructor that takes a DWORD to ensure
        // that the timer can only be used with relative times.
        class time_type
        {
        public:
            time_type() : ticks_(0) {}
        private:
            friend class time_traits_type;
            DWORD ticks_;
        };

        // Get the current time.
        static time_type now()
        {
            time_type result;
            result.ticks_ = ::GetTickCount();
            return result;
        }

        // Add a duration to a time.
        static time_type add(const time_type& t, const duration_type& d)
        {
            time_type result;
            result.ticks_ = t.ticks_ + d.ticks_;
            return result;
        }

        // Subtract one time from another.
        static duration_type subtract(const time_type& t1, const time_type& t2)
        {
            return duration_type(t1.ticks_ - t2.ticks_);
        }

        // Test whether one time is less than another.
        static bool less_than(const time_type& t1, const time_type& t2)
        {
            // DWORD tick count values wrap periodically, so we'll use a heuristic that
            // says that if subtracting t1 from t2 yields a value smaller than 2^31,
            // then t1 is probably less than t2. This means that we can't handle
            // durations larger than 2^31, which shouldn't be a problem in practice.
            return (t2.ticks_ - t1.ticks_) < static_cast<DWORD>(1 << 31);
        }

        // Convert to POSIX duration type.
        static boost::posix_time::time_duration to_posix_duration(
            const duration_type& d)
        {
            return boost::posix_time::milliseconds(d.ticks_);
        }
    };

    typedef time_traits_type::duration_type duration_type;
    typedef time_traits_type::time_type time_type;
};


class timer_traits
{
public:
    class time_traits_type
    {
    public:
        // The duration type.
        class duration_type
        {
        public:
            duration_type() : ticks_(0) {}
            explicit duration_type(int64_t ticks) : ticks_(ticks) {}
        private:
            friend class time_traits_type;
            int64_t ticks_;
        };

        // The time type. This type has no constructor that takes a DWORD to ensure
        // that the timer can only be used with relative times.
        class time_type
        {
        public:
            time_type() : ticks_(0) {}
        private:
            friend class time_traits_type;
            int64_t ticks_;
        };

        // Get the current time.
        static time_type now()
        {
            time_type result;
            result.ticks_ = detail::accurate_tick_counter::get_current_count();
            return result;
        }

        // Add a duration to a time.
        static time_type add(const time_type& t, const duration_type& d)
        {
            time_type result;
            result.ticks_ = t.ticks_ + d.ticks_;
            return result;
        }

        // Subtract one time from another.
        static duration_type subtract(const time_type& t1, const time_type& t2)
        {
            return duration_type(t1.ticks_ - t2.ticks_);
        }

        // Test whether one time is less than another.
        static bool less_than(const time_type& t1, const time_type& t2)
        {
            return t1.ticks_ < t2.ticks_;
        }

        // Convert to POSIX duration type.
        static boost::posix_time::time_duration to_posix_duration(
            const duration_type& d)
        {
            return boost::posix_time::milliseconds(d.ticks_);
        }
    };

    typedef time_traits_type::duration_type duration_type;
    typedef time_traits_type::time_type time_type;
};


} }


