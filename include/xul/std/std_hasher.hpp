#pragma once

#include <boost/functional/hash.hpp>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


namespace xul {


class std_hasher
{
public:
    static size_t hash_bytes(const uint8_t* data, size_t size)
    {
        return boost::hash_range(data, data + size);
    }
};

}
