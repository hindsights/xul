#pragma once

#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace xul { namespace detail {

class timer_traits
{
public:
    typedef boost::posix_time::ptime time_type;
    typedef boost::asio::time_traits<boost::posix_time::ptime> time_traits_type;
    typedef boost::posix_time::milliseconds duration_type;
};

} }


