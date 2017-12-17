#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class io_server_listener;
class io_session;


/// like IoService in apache mina, not boost::asio::io_service
class io_server : public object
{
public:
    virtual void set_listener(io_server_listener* listener) = 0;
};


class io_server_listener
{
public:
    virtual bool on_session_create(io_server* sender, io_session* session) = 0;
    virtual void on_session_destroy(io_server* sender, io_session* session, int errcode) { }
};


}
