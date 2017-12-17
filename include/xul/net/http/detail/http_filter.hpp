#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>


namespace xul {


class url_request;
class url_response;

class http_filter : public object
{
public:
    virtual const char* name() const = 0;
    virtual void set_next(http_filter* next_filter) = 0;
    virtual bool init(url_request* req, url_response* resp) = 0;
    virtual bool process(url_request* req, url_response* resp, const uint8_t* data, int size) = 0;
    virtual bool finish(url_request* req, url_response* resp, const uint8_t* data, int size) = 0;
    //virtual void abort(url_request* req, url_response* resp) = 0;
    virtual void handle_error(url_request* req, url_response* resp, int errcode) = 0;
};


}
