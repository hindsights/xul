#pragma once

#include <xul/lang/object.hpp>
#include <xul/net/url_request.hpp>
#include <xul/net/http/http_connection_listener.hpp>
#include <xul/net/http/http_connection_options.hpp>
#include <xul/data/string_table.hpp>


namespace xul {


class inet_socket_address;


class http_error_codes
{
public:
    enum error_code
    {
        invalid_url = -1, 
        invalid_range_size = -2, 
        invalid_redirect_url = -3, 
        redirect_too_many_times = -4, 
        invalid_response_header = -101, 
    };
};


class http_connection : public object
{
public:
    //virtual bool execute(const char* host, int port, const url_request* req) = 0;
    virtual bool request(const char* method, const char* urlstr, const string_table* headers) = 0;
    virtual bool send_data(const uint8_t* data, int size) = 0;
    virtual bool download(const char* urlstr, int64_t pos, int64_t len, const string_table* headers) = 0;

    virtual void set_listener(http_connection_listener* listener) = 0;
    virtual void set_handler(http_connection_handler* listener) = 0;
    virtual const http_connection_listener* get_listener() const = 0;
    virtual const http_connection_options* get_options() const = 0;
    virtual http_connection_options* ref_options() = 0;

    virtual bool is_started() const = 0;
    virtual bool is_requesting() const
    {
        return this->is_started();
    }
    virtual bool is_open() const = 0;
    virtual bool is_connected() const
    {
        return this->is_open();
    }
    virtual bool is_paused() const = 0;
    virtual void close() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void destroy() = 0;
    //virtual void set_headers(const string_table* headers) = 0;
    virtual void set_receive_buffer_size(int bufsize) = 0;
    virtual void set_decoder_buffer_size(int bufsize) { }
    virtual const inet_socket_address& get_local_address() const = 0;
    virtual const inet_socket_address& get_remote_address() const = 0;
};


class io_service;
http_connection* create_http_connection(io_service* ios);
http_connection* create_concurrent_http_connection(io_service* ios);


}
