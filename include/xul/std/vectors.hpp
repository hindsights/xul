#pragma once

#include <xul/std/containers.hpp>
#include <vector>

namespace xul {


class vectors
{
public:
    template <typename ValueT, typename AllocatorT>
    static bool contains(const std::vector<ValueT, AllocatorT>& vec, const ValueT& val)
    {
        return sequences::contains(vec.begin(), vec.end(), val);
    }
};

}

