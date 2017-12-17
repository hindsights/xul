#pragma once

#include <xul/net/http/http_connection.hpp>
#include <xul/net/http/detail/http_filter.hpp>


namespace xul { namespace detail {


class http_connection_request_info;


class inner_http_connection : public http_connection
{
public:
    virtual bool redirect(http_connection_request_info* reqInfo) = 0;
    virtual void handle_error(int errcode) = 0;
    virtual void abort_handling() = 0;
    virtual bool on_error(int errcode) = 0;
    virtual bool on_header(url_request* req, url_response* resp) = 0;
    virtual bool on_content(url_request* req, url_response* resp, const uint8_t* data, int size) = 0;
    virtual bool on_complete(url_request* req, url_response* resp, int64_t total_size, const uint8_t* data, int size) = 0;
    virtual xul::logger* get_logger() const = 0;
};


} }
