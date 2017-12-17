#pragma once

/**
 * @file
 * @brief bmf(binary message format)
 */

#include <xul/io/bmf_integer_encoding.hpp>
#include <xul/io/output_stream.hpp>
#include <boost/noncopyable.hpp>
#include <stddef.h>
#include <stdint.h>


namespace xul {


/// bmf(binary message format)
class bmf_format
{
public:
    enum
    {
        integer_tag = 1,
        double_tag = 4,
        string_tag = 7,
        list_tag = 10,
        dict_tag = 13,
        end_tag = 17
    };
};


}
