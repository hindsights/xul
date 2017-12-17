#pragma once

#include <xul/net/tcp_socket_listener.hpp>
#include <xul/log/log.hpp>
#include <assert.h>

namespace xul {
namespace detail {


class dummy_tcp_socket_listener : public tcp_socket_listener
{
public:
    virtual void on_socket_connect(tcp_socket* sender)
    {
        XUL_APP_WARN("on_socket_connect ignored " << sender);
    }
    virtual void on_socket_connect_failed(tcp_socket* sender, int errcode)
    {
        XUL_APP_WARN("on_socket_connect_failed ignored " << xul::make_tuple(sender, errcode));
    }

    virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)
    {
        XUL_APP_WARN("on_socket_receive ignored:" << xul::make_tuple(sender, (const void*)data, size));
    }
    virtual void on_socket_receive_failed(tcp_socket* sender, int errcode)
    {
        XUL_APP_WARN("on_socket_receive_failed ignored:" << xul::make_tuple(sender, errcode));
    }

    virtual void on_socket_send(tcp_socket* sender, size_t bytes)
    {
        XUL_APP_WARN("on_socket_send ignored:" << xul::make_tuple(sender, bytes));
    }
    virtual void on_socket_send_failed(tcp_socket* sender, int errcode)
    {
        XUL_APP_WARN("on_socket_send_failed ignored:" << xul::make_tuple(sender, errcode));
    }
};


class checked_tcp_socket_listener : public tcp_socket_listener
{
public:
    virtual void on_socket_connect(tcp_socket* sender)
    {
        XUL_APP_ERROR("on_socket_connect checked " << sender);
        assert(false);
    }
    virtual void on_socket_connect_failed(tcp_socket* sender, int errcode)
    {
        XUL_APP_ERROR("on_socket_connect_failed checked " << xul::make_tuple(sender, errcode));
        assert(false);
    }

    virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)
    {
        XUL_APP_ERROR("on_socket_receive checked:" << xul::make_tuple(sender, (const void*)data, size));
        assert(false);
    }
    virtual void on_socket_receive_failed(tcp_socket* sender, int errcode)
    {
        XUL_APP_ERROR("on_socket_receive_failed checked:" << xul::make_tuple(sender, errcode));
        assert(false);
    }

    virtual void on_socket_send(tcp_socket* sender, size_t bytes)
    {
        // can be ignored
        XUL_APP_ERROR("on_socket_send checked:" << xul::make_tuple(sender, bytes));
    }
    virtual void on_socket_send_failed(tcp_socket* sender, int errcode)
    {
        XUL_APP_ERROR("on_socket_send_failed checked:" << xul::make_tuple(sender, errcode));
        assert(false);
    }
};

}
}
