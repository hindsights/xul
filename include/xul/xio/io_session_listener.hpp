#pragma once

#include <stddef.h>


namespace xul {


class io_session;
class io_session_listener;
class decoder_message_base;
class message_decoder;


class io_session_listener
{
public:
    virtual void on_session_open(io_session* session) { }
    virtual void on_session_close(io_session* session) { }
    virtual void on_session_error(io_session* session, int errcode) { }
    virtual void on_session_receive(io_session* session, xul::decoder_message_base* msg) { }
    virtual void on_session_send(io_session* session, size_t bytes) { }

    virtual message_decoder* create_message_decoder() = 0;
};

class io_session_handler : public object, public io_session_listener
{
};


}
