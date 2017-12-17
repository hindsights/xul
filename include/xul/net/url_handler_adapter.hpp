#pragma once

#include <xul/net/url_handler.hpp>
#include <xul/net/url_session.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


class url_handler_adapter : public object_impl<url_handler>
{
public:
    virtual void handle_request(url_session* session, const url_request* req)
    {
        XUL_APP_REL_WARN("url_handler_adapter.handle_request reject and close " << session);
        session->close();
    }
    virtual void handle_body_data(url_session* session, const uint8_t* data, int size)
    {
        XUL_APP_REL_WARN("url_handler_adapter.handle_request reject and close " << session << " " << size);
        session->close();
    }
    virtual void handle_error(url_session* session, int errcode)
    {
        XUL_APP_REL_WARN("url_handler_adapter.handle_error reject and close " << session << " " << errcode);
        session->close();
    }
    virtual void handle_data_sent(url_session* session, int bytes)
    {
    }
    virtual void handle_request_finished(url_session* session)
    {
    }
};


}
