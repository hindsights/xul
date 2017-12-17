#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class io_service;
class tcp_server;
class http_server;
class http_connection;
class http_client;

#if 0
class net_factory : public object
{
public:
    virtual io_service* get_io_service() = 0;
    virtual tcp_server* create_tcp_server() = 0;
    virtual http_server* create_http_server() = 0;
    virtual http_connection* create_http_connection() = 0;
    virtual http_client* create_http_client() = 0;
};


net_factory* create_net_factory(io_service* ios);
#endif

}
