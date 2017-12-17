#pragma once

#include <xul/lang/object.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/io/codec/buffered_message_decoder.hpp>
#include <xul/data/bit_converter.hpp>
#include <xul/data/buffer.hpp>
#include <vector>


namespace xul {


class data_codec;

class data_codec_callback
{
public:
    virtual int on_data_codec_buffer(data_codec*, const uint8_t*, size_t) = 0;
};

class data_codec : public object
{
public:
    virtual int feed(const uint8_t* data, int size, data_codec_callback* callback) = 0;
    virtual int flush(data_codec_callback* callback) = 0;
    virtual void reset() = 0;
    virtual const uint8_t* get_remaining_data() const = 0;
    virtual int get_remaining_size() const = 0;
};


}
