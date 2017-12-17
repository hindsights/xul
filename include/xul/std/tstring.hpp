#pragma once

#include <boost/config.hpp>

#include <string>

#include <xul/data/tchar.hpp>

#if defined(BOOST_WINDOWS)
inline int strncasecmp(const char* s1, const char* s2, size_t size)
{
    return _strnicmp(s1, s2, size);
}
#else
#include <strings.h>
#endif


namespace xul {

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

}

