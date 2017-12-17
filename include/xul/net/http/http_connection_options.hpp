#pragma once

#include <xul/lang/object.hpp>
#include <xul/util/options.hpp>


namespace xul {


class http_connection_options : public options
{
public:
    virtual bool is_keep_alive() const = 0;
    virtual void set_keep_alive(bool keep_alive) = 0;

    virtual int get_max_redirects() const = 0;
    virtual void set_max_redirects(int count) = 0;

    virtual const char* get_protocol_name() const = 0;
    virtual void set_protocol_name(const char* protocol_name) = 0;

    virtual const char* get_protocol_version() const = 0;
    virtual void set_protocol_version(const char* protocol_version) = 0;

    virtual void set_concurrent_requests(bool enabled) = 0;
    virtual bool is_concurrent_requests_enabled() const = 0;

    virtual bool is_gzip_enabled() const = 0;

    virtual void set_receive_buffer_size(int bufsize) = 0;
    //virtual void set_decoder_buffer_size(int bufsize) = 0;
};


}
