#pragma once

#include <boost/config.hpp>


#include <xul/data/string.hpp>

#include <wchar.h>
#include <stdio.h>


#if defined(BOOST_NO_CWCHAR)
#ifndef _WCHAR_T_DEFINED

typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#endif


#if defined(BOOST_NO_STD_WSTRING)

namespace std {

typedef basic_string<wchar_t> wstring;

}

#endif




#if defined(BOOST_WINDOWS)

#include <tchar.h>

#else

#if defined(UNICODE) || defined(_UNICODE)

#define __T(x)      L ## x
typedef wchar_t TCHAR;

#else

#define __T(x)      x

#endif

typedef char TCHAR;

#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)
#define TEXT(x) _T(x)

#endif


typedef const char* LPCSTR;
typedef char* LPSTR;

typedef const TCHAR* LPCTSTR;
typedef TCHAR* LPTSTR;



