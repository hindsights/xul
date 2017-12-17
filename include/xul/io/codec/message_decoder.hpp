#pragma once

#include <xul/io/codec/message_decoder_listener.hpp>
#include <xul/lang/object.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class message_decoder : public object
{
public:
    virtual void set_listener(message_decoder_listener* listener) = 0;

    virtual bool feed(const uint8_t* data, int size) = 0;

    virtual void reset() = 0;

    virtual void abort() = 0;

    virtual bool is_aborted() const = 0;

    virtual const uint8_t* get_remaining_data() const = 0;

    virtual int get_remaining_size() const = 0;
};


}
