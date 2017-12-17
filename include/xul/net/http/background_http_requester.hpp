#pragma once

#include <xul/net/io_service.hpp>
#include <xul/net/http/http_client.hpp>
#include <xul/lang/object.hpp>


namespace xul {


class background_http_requester : public object
{
public:
    virtual void set_common_header(const char* key, const char* val) = 0;
    //virtual http_client* request(url_request* req, const char* body, int bodysize) = 0;
    virtual http_client* post(const char* url, const std::string& body, const std::string& content_type) = 0;
    virtual http_client* notify(const char* url) = 0;
};


background_http_requester* create_background_http_requester(io_service* ios);


}
