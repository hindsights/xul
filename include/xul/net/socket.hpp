#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>


namespace xul {


class socket_address;


/// basic bsd socket
class socket : public object
{
public:
    virtual int get_handle() const = 0;
    virtual bool is_open() const = 0;
    virtual void close() = 0;
    virtual bool listen(int backlog) = 0;
    virtual bool create(int domain, int type, int protocol) = 0;
    virtual bool ioctl(int cmd, char* arg) = 0;
    virtual bool set_option(int level, int cmd, const void* data, int size) = 0;

    virtual bool get_local_address(socket_address& addr) const = 0;
    virtual bool get_remote_address(socket_address& addr) const = 0;

    virtual int get_last_error() const = 0;

    virtual bool connect(const socket_address& addr) = 0;
    virtual int send(const uint8_t* data, int size) = 0;
    virtual int send_n(const uint8_t* data, int size) = 0;
    virtual int receive(uint8_t* data, int size) = 0;
    virtual int receive_n(uint8_t* data, int size) = 0;
    virtual int accept(socket_address& addr) = 0;
    virtual bool bind(const socket_address& addr) = 0;
};


}
