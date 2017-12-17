#pragma once

#include <xul/net/url_message.hpp>
#include <xul/net/uri.hpp>


namespace xul {


/// A request received from a client.
class url_request : public url_message
{
public:
    virtual url_request* clone() const = 0;
    virtual const char* get_method() const = 0;
    virtual void set_method(const char* m) = 0;

    virtual const uri* get_uri() const = 0;
    virtual const char* get_url() const = 0;
    virtual void set_url(const char* s) = 0;
};


url_request* create_url_request();


}
