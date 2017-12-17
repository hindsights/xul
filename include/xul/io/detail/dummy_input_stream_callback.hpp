#pragma once

#include <xul/io/input_stream_callback.hpp>
#include <stddef.h>
#include <stdint.h>

namespace xul {
namespace detail {


class dummy_input_stream_callback : public input_stream_callback
{
public:
    virtual void on_data_read(input_stream* sender, int errcode, const uint8_t* data, size_t size, int64_t pos)
    {
        // ignore
    }

    static input_stream_callback* instance()
    {
        static dummy_input_stream_callback dummyCallback;
        return &dummyCallback;
    }
};

class check_input_stream_callback : public input_stream_callback
{
public:
    virtual void on_data_read(input_stream* sender, int errcode, const uint8_t* data, size_t size, int64_t pos)
    {
        assert(false);
    }

    static input_stream_callback* instance()
    {
        static dummy_input_stream_callback checkCallback;
        return &checkCallback;
    }
};

}
}

