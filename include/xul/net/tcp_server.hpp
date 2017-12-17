#pragma once

#include <xul/xio/io_server.hpp>
#include <xul/xio/io_session.hpp>


namespace xul {


class tcp_server : public io_server
{
public:
    virtual bool start(int port) = 0;
    virtual void stop() = 0;
    virtual void enable_reuse_address(bool enabled) = 0;
    virtual int get_session_count() const = 0;
    virtual bool feed_client(tcp_socket* sock) = 0;
};


}
