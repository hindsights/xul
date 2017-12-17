#pragma once

#include <xul/lang/object.hpp>
#include <xul/net/message_socket_listener.hpp>
#include <stdint.h>


namespace xul {


class message_socket : public object
{
public:
    virtual void set_listener(message_socket_listener* listener) = 0;
    virtual message_socket_listener* get_listener() const = 0;

    virtual void close() = 0;
    virtual bool is_open() const = 0;

    virtual int get_handle() const = 0;

    virtual void destroy() = 0;

    /// receive at most 'maxSize' bytes of data
    virtual void receive(size_t size, size_t ancillary_data_len, int flags) = 0;
    virtual void receive_from(size_t size, size_t ancillary_data_len, size_t addr_len, int flags) = 0;
    /// receive exactly 'size' bytes of data
    //virtual void receive_n(size_t size) = 0;

    /// send at most 'size' bytes of data
    virtual bool send(const void* data, size_t size) = 0;
    /// send all the 'size' bytes of data
    //virtual bool send_n(const void* data, size_t size) = 0;

    virtual bool is_sending() const = 0;
    virtual bool is_receiving() const = 0;

    virtual void set_max_sending_queue_size(int queue_size) = 0;
    virtual int get_max_sending_queue_size() const = 0;
    virtual int get_sending_queue_size() const = 0;
};


}
