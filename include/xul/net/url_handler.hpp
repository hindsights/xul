#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>


namespace xul {


class message_decoder;
class url_request;
class url_session;


/// handles a single url request from client
class url_handler : public object
{
public:
    virtual void handle_request(url_session* session, const url_request* req) = 0;
    virtual void handle_body_data(url_session* session, const uint8_t* data, int size) = 0;
    virtual void handle_error(url_session* session, int errcode) = 0;
    virtual void handle_data_sent(url_session* session, int bytes) = 0;
    virtual void handle_request_finished(url_session* session) = 0;
};


}
