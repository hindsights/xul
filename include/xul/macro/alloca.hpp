#pragma once

#include <boost/config.hpp>

#ifdef BOOST_WINDOWS

#include <malloc.h>
#define XUL_ALLOCA(t, n) static_cast<t*>(_alloca(sizeof(t) * (n)))

#else

#ifdef __FreeBSD__
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#define XUL_ALLOCA(t, n) static_cast<t*>(alloca(sizeof(t) * (n)))

#endif
