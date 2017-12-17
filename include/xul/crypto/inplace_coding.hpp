#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>


namespace xul {


class inplace_coding : public object
{
public:
    virtual void process(uint8_t* buf, int size) = 0;
    virtual void transform(uint8_t* dst, const uint8_t* src, int size) = 0;
};


}
