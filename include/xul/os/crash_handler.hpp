#pragma once

#include <boost/config.hpp>

#if defined(BOOST_WINDOWS)
#include <xul/mswin/crash_handler.hpp>

#else

#include <xul/posix/crash_handler.hpp>

#endif


