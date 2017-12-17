#pragma once

#include <boost/config.hpp>


#if defined(BOOST_WINDOWS)

#include <tchar.h>

#define vsntprintf _vsntprintf
#define vsnprintf _vsnprintf

#else

#include <stdio.h>
#define vsntprintf vsnprintf

#endif

