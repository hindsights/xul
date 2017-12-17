#pragma once

#include <xul/config.hpp>

#ifdef XUL_WINDOWS
#pragma warning(disable: 4127)
#endif


#define XUL_JOIN(symbol1, symbol2)   _XUL_DO_JOIN( symbol1, symbol2 )
#define _XUL_DO_JOIN(symbol1, symbol2)   _XUL_DO_JOIN2( symbol1, symbol2 )
#define _XUL_DO_JOIN2(symbol1, symbol2)   symbol1##symbol2

#define XUL_MAKE_UNIQUE_NAME(prefix)   XUL_JOIN( prefix, __LINE__ )
