#pragma once

#include <xul/data/tuple.hpp>

#include <utility>

namespace std {

template <typename T1, typename T2>
inline ostream& operator<<(ostream& os, const pair<T1, T2>& val)
{
    return os << "(" << val.first << "," << val.second << ")";
}

}


namespace xul {

inline std::pair<const void*, size_t> make_buffer_pair(const void* data, size_t size)
{
    return std::make_pair(data, size);
}

}

