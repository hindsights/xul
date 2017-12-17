#pragma once

#include <stdint.h>


namespace xul {


class http_connection;
class url_response;


class http_connection_listener
{
public:
    virtual bool on_http_response(http_connection* sender, url_response* resp, const char* real_url) = 0;
    virtual bool on_http_data(http_connection* sender, const uint8_t* data, int size) = 0;
    virtual void on_http_put_content(http_connection* sender) { }
    virtual void on_http_complete(http_connection* sender, int64_t size) = 0;
    virtual void on_http_error(http_connection* sender, int errcode) = 0;
};


class http_connection_handler : public xul::object, public http_connection_listener
{
};


class http_connection_listener_adapter : public http_connection_listener
{
public:
    virtual void on_http_put_content(http_connection* sender) { }
};


}
