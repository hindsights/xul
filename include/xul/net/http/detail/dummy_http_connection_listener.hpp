#pragma once

#include <xul/net/http/http_connection_listener.hpp>


namespace xul {


class dummy_http_connection_listener : public http_connection_listener
{
public:
    virtual bool on_http_response(http_connection* sender, url_response* resp, const char* real_url) { return false; }
    virtual bool on_http_data(http_connection* sender, const uint8_t* data, int size) { return false; }
    virtual void on_http_put_content(http_connection* sender) { }
    virtual void on_http_complete(http_connection* sender, int64_t size) { }
    virtual void on_http_error(http_connection* sender, int errcode) { }

    static dummy_http_connection_listener* instance()
    {
        static dummy_http_connection_listener dummy_listener;
        return &dummy_listener;
    }
};


}
