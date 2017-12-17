#pragma once

#include <xul/net/message_socket_listener.hpp>
#include <xul/log/log.hpp>
#include <assert.h>

namespace xul {
namespace detail {


class dummy_message_socket_listener : public message_socket_listener
{
public:
    virtual void on_socket_receive(message_socket* sender, const struct msghdr& hdr)
    {
        XUL_APP_WARN("on_socket_receive ignored:" << xul::make_tuple(sender, 0));
    }
    virtual void on_socket_receive_failed(message_socket* sender, int errcode)
    {
        XUL_APP_WARN("on_socket_receive_failed ignored:" << xul::make_tuple(sender, errcode));
    }

    virtual void on_socket_send(message_socket* sender, size_t bytes)
    {
        XUL_APP_WARN("on_socket_send ignored:" << xul::make_tuple(sender, bytes));
    }
    virtual void on_socket_send_failed(message_socket* sender, int errcode)
    {
        XUL_APP_WARN("on_socket_send_failed ignored:" << xul::make_tuple(sender, errcode));
    }
};


class checked_message_socket_listener : public message_socket_listener
{
public:
    virtual void on_socket_receive(message_socket* sender, const struct msghdr& hdr)
    {
        XUL_APP_ERROR("on_socket_receive checked:" << xul::make_tuple(sender, 0));
        assert(false);
    }
    virtual void on_socket_receive_failed(message_socket* sender, int errcode)
    {
        XUL_APP_ERROR("on_socket_receive_failed checked:" << xul::make_tuple(sender, errcode));
        assert(false);
    }

    virtual void on_socket_send(message_socket* sender, size_t bytes)
    {
        // can be ignored
        XUL_APP_ERROR("on_socket_send checked:" << xul::make_tuple(sender, bytes));
    }
    virtual void on_socket_send_failed(message_socket* sender, int errcode)
    {
        XUL_APP_ERROR("on_socket_send_failed checked:" << xul::make_tuple(sender, errcode));
        assert(false);
    }
};


}
}
