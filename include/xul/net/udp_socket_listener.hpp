#pragma once

#include <stddef.h>


namespace xul {


class inet_socket_address;
class udp_socket;


class udp_socket_listener
{
public:
    virtual void on_socket_receive(udp_socket* sender, unsigned char* data, int size, inet_socket_address* remoteAddr)  = 0;

    virtual void on_socket_receive_failed(udp_socket* sender, int errcode) = 0;
    virtual ~udp_socket_listener() {}
};


}
