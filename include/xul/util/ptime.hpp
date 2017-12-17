#pragma once

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>


namespace boost { namespace posix_time {

    inline std::ostream& operator<<(std::ostream& os, const boost::posix_time::ptime& t)
    {
        return os << boost::posix_time::to_simple_string(t);
    }

    inline std::ostream& operator<<(std::ostream& os, const boost::posix_time::time_duration& t)
    {
        return os << boost::posix_time::to_simple_string(t);
    }

} }
