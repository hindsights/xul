#pragma once

#include <boost/config.hpp>

#if !defined(BOOST_WINDOWS)
#error "platform must be mswin "
#endif


#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#endif

// Windows Header Files:
#include <windows.h>
