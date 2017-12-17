#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>


namespace xul {


class udp_socket_listener;
class inet_socket_address;
class socket_address;


class udp_socket : public object
{
public:
    virtual ~udp_socket() { }

    virtual bool is_open() const = 0;
    virtual void close() = 0;
    virtual void set_listener(udp_socket_listener* handler) = 0;

    virtual bool open( uint16_t port ) = 0;

    /// receive a packet, 'maxSize' is the maximum packet size
    virtual void receive( size_t maxSize ) = 0;

    /// send a packet
    bool send(const void* data, size_t size, const xul::inet_socket_address* addr)
    {
        return send(data, size, *addr);
    }
    virtual bool send(const void* data, size_t size, const xul::inet_socket_address& addr) = 0;

    /// retrieve the bound local address
    virtual inet_socket_address get_local_address() const = 0;

    /// retrieve the bound local address
    virtual bool get_local_address(socket_address* addr) const = 0;

};


}

