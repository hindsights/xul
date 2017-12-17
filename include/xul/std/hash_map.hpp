#pragma once

#include <boost/config.hpp>


#if defined(BOOST_WINDOWS)
#include <hash_map>

namespace xul {
    using stdext::hash_map;
}
#elif defined(BOOST_HAS_HASH)
#include BOOST_HASH_MAP_HEADER

namespace xul {
    using BOOST_STD_EXTENSION_NAMESPACE::hash_map;
}

#else
#error no hash_map
#endif
