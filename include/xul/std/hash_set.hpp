#pragma once

#include <boost/config.hpp>


#if defined(BOOST_WINDOWS)
#include <hash_set>

namespace xul {
    using stdext::hash_set;
}
#elif defined(BOOST_HAS_HASH)
#include BOOST_HASH_SET_HEADER

namespace xul {
    using BOOST_STD_EXTENSION_NAMESPACE::hash_set;
}

#else
#error no hash_set
#endif
