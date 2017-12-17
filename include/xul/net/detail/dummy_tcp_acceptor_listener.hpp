#pragma once

#include <xul/net/tcp_acceptor_listener.hpp>
#include <assert.h>


namespace xul {
namespace detail {


class dummy_tcp_acceptor_listener : public tcp_acceptor_listener
{
public:
    virtual void on_acceptor_client(tcp_acceptor* acceptor, tcp_socket* newClient, const inet_socket_address& sockAddr)
    {
    }
};


class checked_tcp_acceptor_listener : public tcp_acceptor_listener
{
public:
    virtual void on_acceptor_client(tcp_acceptor* acceptor, tcp_socket* newClient, const inet_socket_address& sockAddr)
    {
        assert(false);
    }
};


}
}
