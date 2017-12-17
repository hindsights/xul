#pragma once

#include <xul/lang/object.hpp>
#include <xul/net/tcp_socket_listener.hpp>
#include <xul/net/socket_address.hpp>
#include <xul/data/buffer.hpp>
#include <stdint.h>

namespace xul {


class tcp_socket : public object
{
public:
    virtual void set_listener(tcp_socket_listener* listener) = 0;
    virtual void reset_listener() = 0;
    virtual tcp_socket_listener* get_listener() const = 0;

    virtual bool bind(const socket_address& addr) = 0;
    virtual bool bind_port(int port) = 0;

    virtual void close() = 0;
    virtual bool is_open() const = 0;
    virtual bool is_connected() const = 0;

    virtual int get_handle() const = 0;

    virtual void destroy() = 0;

    /// receive at most 'maxSize' bytes of data
    virtual void receive(size_t maxSize) = 0;
    /// receive exactly 'size' bytes of data
    virtual void receive_n(size_t size) = 0;

    /// send at most 'size' bytes of data
    virtual bool send(const void* data, size_t size) = 0;
    /// send all the 'size' bytes of data
    virtual bool send_n(const void* data, size_t size) = 0;

    virtual bool is_sending() const = 0;
    virtual bool is_receiving() const = 0;

    virtual bool connect(const socket_address& addr) = 0;

    virtual void set_max_sending_queue_size(int queue_size) = 0;
    virtual int get_max_sending_queue_size() const = 0;
    virtual int get_sending_queue_size() const = 0;

    virtual void set_max_writing_queue_size(int queue_size) = 0;
    virtual int get_max_writing_queue_size() const = 0;
    virtual int get_writing_queue_size() const = 0;

    virtual bool get_local_address(socket_address& addr) const = 0;
    virtual bool get_remote_address(socket_address& addr) const = 0;
};


}
