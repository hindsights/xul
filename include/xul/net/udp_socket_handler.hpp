#pragma once

#include <xul/lang/object.hpp>
#include <stddef.h>


namespace xul {


class udp_socket_handler;
class inet_socket_address;
class udp_socket;


class udp_socket_handler : public xul::object
{
public:
    virtual void on_socket_receive(udp_socket* sender, unsigned char* data, size_t size, inet_socket_address* remoteAddr)  = 0;

    virtual void on_socket_receive_failed(udp_socket* sender, int errcode) = 0;
};


}
