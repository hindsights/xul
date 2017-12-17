#pragma once

#include <xul/xio/net/url_handler.hpp>
#include <xul/xio/net/http_request_decoder.hpp>
#include <stdint.h>


namespace xul {


class http_request_handler;

/// http
class http_request_handler : public io_session_listener, public url_handler
{
public:
    http_request_handler()
    {
    }
    virtual ~http_request_handler() { }

    virtual message_decoder_ptr create_message_decoder()
    {
        return http_request_decoder::create();
    }

    virtual void on_session_receive(io_session* session, message_decoder* decoder, xul::decoder_message_base* msg)
    {
        http_request_decoder::decoder_message* realMsg = static_cast<http_request_decoder::decoder_message*>(msg);
        if (realMsg->request)
            handle_request(session, decoder, *realMsg->request);
        else
            handle_body_data(session, decoder, realMsg->data, realMsg->size);
    }


    virtual void on_session_open(io_session* session)
    {
        //session->read();
    }

    virtual void on_session_send(io_session* session, size_t bytes)

    {

        on_response_send(session, bytes);

    }

};


}
