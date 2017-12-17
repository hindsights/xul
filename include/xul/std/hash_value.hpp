#pragma once

#include <boost/config.hpp>

#if defined(BOOST_WINDOWS)

#include <hash_set>

#elif defined(BOOST_HASH_SET_HEADER)

#include BOOST_HASH_SET_HEADER

#else
#error no hash_func
#endif
