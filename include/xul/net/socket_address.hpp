#pragma once

#include <xul/config.hpp>
#include <xul/data/printable.hpp>

#if defined(XUL_WINDOWS)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif


namespace xul {


class socket_address : public printable
{
public:
    virtual ~socket_address() { }

    virtual sockaddr* get_data() = 0;
    virtual const sockaddr* get_data() const = 0;

    virtual int get_length() const = 0;
};


}
