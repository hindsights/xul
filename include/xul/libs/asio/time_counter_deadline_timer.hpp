#pragma once

#include <xul/util/time_counter.hpp>


namespace xul { namespace detail {

class timer_traits
{
public:
    typedef xul::detail::tick_counter64 impl_type;
    typedef impl_type::count_value_type count_value_type;
    class time_traits_type
    {
    public:
        // The time type. This type has no constructor that takes a DWORD to ensure
        // that the timer can only be used with relative times.
        class time_type
        {
        public:
            time_type() : ticks_(0) {}
        private:
            friend class time_traits_type;
            count_value_type ticks_;
        };


    // The duration type.
    class duration_type
    {
    public:
        duration_type() : ticks_(0) {}
        explicit duration_type(count_value_type ticks) : ticks_(ticks) {}
    private:
        friend class time_traits_type;
        count_value_type ticks_;
    };

        // Get the current time.
        static time_type now()
        {
            time_type result;
            result.ticks_ = impl_type::get_current_count();
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
            return t1.ticks_ < t2.ticks_;
        }

        // Convert to POSIX duration type.
        static boost::posix_time::time_duration to_posix_duration(
            const duration_type& d)
        {
            return boost::posix_time::milliseconds(d.ticks_);
        }
    };

    typedef time_traits_type::time_type time_type;
    typedef time_traits_type::duration_type duration_type;
};


} }


