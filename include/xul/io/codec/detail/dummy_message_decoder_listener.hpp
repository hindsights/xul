#pragma once

#include <xul/io/codec/message_decoder_listener.hpp>
#include <assert.h>


namespace xul {
namespace detail {


class dummy_message_decoder_listener : public message_decoder_listener
{
public:
    virtual void on_decoder_message(message_decoder* decoder, decoder_message_base* msg) { }
    virtual void on_decoder_error(message_decoder* decoder, int errcode) { }
};


class checked_message_decoder_listener : public message_decoder_listener
{
public:
    virtual void on_decoder_message(message_decoder* decoder, decoder_message_base* msg) { assert(false); }
    virtual void on_decoder_error(message_decoder* decoder, int errcode) { assert(false); }
};


}
}
