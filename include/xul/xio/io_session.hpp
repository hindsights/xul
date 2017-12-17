#pragma once

#include <xul/lang/object.hpp>
#include <xul/xio/io_session_listener.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class io_session_listener;
class io_session;
class message_decoder;
class socket_address;


class io_session : public object
{
public:
    virtual void set_listener(io_session_listener* listener) = 0;
    virtual void set_handler(io_session_handler* handler) = 0;

    virtual void set_message_decoder(message_decoder* decoder) = 0;
    virtual message_decoder* get_message_decoder() = 0;

    virtual bool write(const uint8_t* data, size_t size) = 0;
    virtual void read(size_t size) = 0;
    /// read a message
    virtual void read() = 0;
    virtual bool write_n(const uint8_t* data, size_t size) = 0;
    virtual void read_n(size_t size) = 0;

    virtual int get_writing_queue_size() const = 0;
    virtual void set_max_writing_queue_size(int queue_size) const = 0;

    virtual bool is_open() const = 0;
    //virtual bool is_writable() const = 0;
    virtual bool is_readable() const = 0;

    virtual bool get_local_address(socket_address& addr) const = 0;
    virtual bool get_remote_address(socket_address& addr) const = 0;
    //virtual inet_socket_address get_local_address() const = 0;
    //virtual inet_socket_address get_remote_address() const = 0;

    //virtual void open() { }
    virtual void close() { }
    virtual void destroy() { }

    virtual void abort() { }
};


}
