#pragma once

#include <xul/net/socket.hpp>
#include <xul/config.hpp>
#if defined(XUL_WINDOWS)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#endif


namespace xul {


/// basic bsd socket
class sockets
{
public:
    static bool open_tcp_server(socket& sock, const socket_address& addr)
    {
        int flag = 1;
        return sock.create(AF_INET, SOCK_STREAM, 0) && sock.set_option(SOL_SOCKET, SO_REUSEADDR, &flag, 4) && sock.bind(addr) && sock.listen(5);
    }
};


}
