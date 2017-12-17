#pragma once

#include <xul/lang/object.hpp>
#include <xul/net/tcp_acceptor_listener.hpp>
#include <xul/net/inet_socket_address.hpp>
#include <stdint.h>
#include <assert.h>

namespace xul {

class tcp_acceptor : public object
{
public:
    virtual ~tcp_acceptor() { }

    virtual void set_listener(tcp_acceptor_listener* listener) = 0;
    virtual void reset_listener() = 0;
    virtual tcp_acceptor_listener* get_listener() const = 0;
    virtual bool is_open() const = 0;

    virtual bool get_local_address(xul::socket_address& addr) const = 0;
    virtual inet_socket_address get_local_address() const = 0;

    virtual void close() = 0;

    virtual bool open(uint16_t port, bool enableReuseAddress) = 0;
    virtual bool feed_client(tcp_socket* sock) = 0;
};

}
