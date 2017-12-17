#pragma once

#include <xul/lang/object.hpp>

namespace boost {
    template <typename R>
    class function0;
}


namespace xul {


class url_handler;
class http_server;
class url_session;
class url_request;


class http_server_router : public object
{
public:
    virtual url_handler* create_url_handler(http_server* server, url_session* session, const url_request* req) = 0;
};


class http_server_route_handler : public object
{
public:
    virtual url_handler* create_url_handler() = 0;
};


class simple_http_server_router : public http_server_router
{
public:
    /// add a handler for a specified path pattern
    virtual void add(const char* pattern, http_server_route_handler* handler) = 0;
    /// set customized not-found handler
    virtual void set_not_found_handler(http_server_route_handler* handler) {}
};

simple_http_server_router* create_simple_http_server_router();
http_server_route_handler* create_http_server_route_handler(boost::function0<url_handler*> f);


}
