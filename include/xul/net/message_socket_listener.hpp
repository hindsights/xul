#pragma once

#include <stddef.h>
#include <assert.h>

struct msghdr;

namespace xul {


class message_socket_listener;
class message_socket;


class message_socket_listener
{
public:
    virtual ~message_socket_listener() { }

    virtual void on_socket_receive(message_socket* sender, const struct msghdr& hdr) = 0;
    virtual void on_socket_receive_failed(message_socket* sender, int errcode) = 0;

    virtual void on_socket_send(message_socket* sender, size_t bytes) { }    // can be ignored
    virtual void on_socket_send_failed(message_socket* sender, int errcode) { this->on_socket_receive_failed(sender, errcode); }
};


}
