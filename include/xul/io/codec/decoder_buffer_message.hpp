#pragma once

#include <xul/io/codec/decoder_message_base.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class decoder_buffer_message : public decoder_message_base
{
public:
    const uint8_t* data;
    int size;
    decoder_buffer_message(const uint8_t* d, int s) : data(d), size(s) { }
};

class decoder_mutable_buffer_message : public decoder_message_base
{
public:
    uint8_t* data;
    int size;
    decoder_mutable_buffer_message(uint8_t* d, int s) : data(d), size(s) { }
};


}
