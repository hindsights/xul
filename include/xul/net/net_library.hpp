#pragma once

#include <xul/lib/library.hpp>


namespace xul {


class io_service;
class net_factory;
class tcp_server;
class http_server;
class http_connection;
class http_client;
class url_request;
class url_response;


class net_library : public library
{
public:
    virtual url_request* create_url_request() = 0;
    virtual url_response* create_url_response() = 0;
    virtual net_factory* create_factory(io_service* ios) = 0;
    virtual tcp_server* create_tcp_server(io_service* ios) = 0;
    virtual http_server* create_http_server(io_service* ios) = 0;
    virtual http_connection* create_http_connection(io_service* ios) = 0;
    virtual http_client* create_http_client(io_service* ios) = 0;
};


net_library* get_net_library();


}
