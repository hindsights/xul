#pragma once

#include <xul/io/codec/decoder_message_base.hpp>


namespace xul {


class message_decoder;


class message_decoder_listener
{
public:
    virtual void on_decoder_message(message_decoder* decoder, decoder_message_base* msg) = 0;

    virtual void on_decoder_error(message_decoder* decoder, int errcode) = 0;
};


}
