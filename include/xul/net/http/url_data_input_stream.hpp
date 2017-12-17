#pragma once

#include <string>
#include <stdint.h>


namespace xul {


class http_request;
class url_data_input_stream_listener;
class io_session;

class url_data_input_stream
{
public:
    virtual ~url_data_input_stream() { }

    virtual void open(io_session* session, const http_request& req) = 0;
    virtual bool read() = 0;
    //virtual void set_listener(url_data_input_stream_listener* listener) = 0;
    //virtual void reset_listener() = 0;
};


class url_data_input_stream_listener
{
public:
    virtual ~url_data_input_stream_listener() { }

    virtual void on_stream_open(url_data_input_stream*, const std::string& content_type, int64_t total_size) = 0;
    virtual void on_stream_open_failed(url_data_input_stream*, int errcode) = 0;
    virtual void on_stream_read(url_data_input_stream*, const uint8_t*, size_t size) = 0;
    virtual void on_stream_read_failed(url_data_input_stream*, int errcode) = 0;
    virtual void on_stream_finished(url_data_input_stream*) = 0;
};


}
