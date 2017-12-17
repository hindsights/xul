#pragma once

/**
 * @file
 * @brief guid functions
 */

#include <boost/config.hpp>

#if defined(BOOST_WINDOWS)

#include <objbase.h>

#pragma comment(lib, "ole32.lib")


#else

//#include <uuid/uuid.h>



#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
#endif

extern const GUID GUID_NULL;

#define XUL_DEFINE_GUID_NULL extern const GUID GUID_NULL = { 0 }

inline bool operator==(const GUID& guidOne, const GUID& guidOther)
{
    return 0 == memcmp(&guidOne, &guidOther, sizeof(GUID));
}

inline int operator!=(const GUID& guidOne, const GUID& guidOther)
{
    return !(guidOne == guidOther);
}

#endif

