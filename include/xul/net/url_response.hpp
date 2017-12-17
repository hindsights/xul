#pragma once

#include <xul/net/url_message.hpp>


namespace xul {


/// A response received from a client.
class url_response : public url_message
{
public:
    virtual int get_status_code() const = 0;
    virtual void set_status_code(int status_code) = 0;

    virtual const char* get_status_description() const = 0;
    virtual void set_status_description(const char* val) = 0;
};


url_response* create_url_response();


}
