#pragma once

#include <xul/net/http/http_server_router.hpp>
#include <xul/lang/object.hpp>


namespace xul {


class tcp_socket;
class io_service;


class http_server : public object
{
public:
    virtual void set_router(http_server_router*) = 0;
    virtual bool start(int port) = 0;
    virtual void stop() = 0;
    virtual void enable_reuse_address(bool enabled) = 0;
    virtual int get_session_count() const = 0;
    virtual bool feed_client(tcp_socket* sock) = 0;
};


http_server* create_http_server(io_service* ios);


}
