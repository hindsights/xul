#pragma once

#include <stddef.h>
#include <assert.h>
#include <xul/lang/object.hpp>
#include <xul/lang/object_impl.hpp>

namespace xul {


class tcp_socket_listener;
class tcp_socket;


class tcp_socket_listener 
{
public:
    virtual ~tcp_socket_listener() { }

    virtual void on_socket_connect(tcp_socket* sender) { assert(false); }
    virtual void on_socket_connect_failed(tcp_socket* sender, int errcode) { assert(false); }

    virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)  = 0;
    virtual void on_socket_receive_failed(tcp_socket* sender, int errcode) = 0;

    virtual void on_socket_send(tcp_socket* sender, size_t bytes) { }    // can be ignored
    virtual void on_socket_send_failed(tcp_socket* sender, int errcode) { this->on_socket_receive_failed(sender, errcode); }
};

class tcp_socket_handler : public object, public tcp_socket_listener
{

};


}
