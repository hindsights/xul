#pragma once

#include <stddef.h>
#include <stdint.h>

namespace xul {


class input_stream;

class input_stream_callback
{
public:
    virtual ~input_stream_callback() { }

    virtual void on_data_read(input_stream* sender, int errcode, const uint8_t* data, size_t size, int64_t pos) = 0;
};

}
