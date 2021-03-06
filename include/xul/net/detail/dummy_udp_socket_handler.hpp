#pragma once

#include <xul/net/udp_socket_handler.hpp>
#include <assert.h>

namespace xul {
namespace detail {


class dummy_udp_socket_handler : public udp_socket_handler
{
public:
    virtual void on_socket_receive(udp_socket* sender, unsigned char* data, size_t size, const inet_socket_address* remoteAddr) { }
    virtual void on_socket_receive_failed(udp_socket* sender, int errcode) { }
};


class checked_udp_socket_handler : public udp_socket_handler
{
public:
    virtual void on_socket_receive(udp_socket* sender, unsigned char* data, size_t size, const inet_socket_address* remoteAddr) { assert(false); }
    virtual void on_socket_receive_failed(udp_socket* sender, int errcode) { assert(false); }
};


}
}
