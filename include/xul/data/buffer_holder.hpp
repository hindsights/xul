#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class buffer_holder : public object
{
public:
    virtual const uint8_t* get_data() const = 0;
    virtual uint8_t* get_buffer() = 0;
    virtual size_t get_size() const = 0;
};


}
