#pragma once

#include <boost/config.hpp>

#include <string.h>
#include <stdio.h>

#if defined(BOOST_WINDOWS)

#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#ifndef snprintf
#define snprintf _snprintf
#endif
#pragma warning(disable: 4996)

#endif

