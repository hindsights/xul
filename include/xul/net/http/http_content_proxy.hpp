#pragma once

#include <xul/net/http/http_connection_listener.hpp>


namespace xul {


class http_response;
class http_connection;
typedef boost::shared_ptr<http_connection> http_connection_ptr;


class http_content_proxy : public http_connection_listener
{
public:
    http_content_proxy()
    {
    }
    virtual ~http_content_proxy()
    {
    }

    virtual void on_http_response(http_connection* sender, const http_response& resp) = 0;
    virtual void on_http_data(http_connection* sender, const uint8_t* data, size_t size)
    {
        if (m_proxy_handler)
            m_proxy_handler->on_http_data(sender, data, size);
    }
    virtual void on_http_complete(http_connection* sender, int64_t size)
    {
        if (m_proxy_handler)
            m_proxy_handler->on_http_complete(sender, size);
    }
    virtual void on_http_error(http_connection* sender, int errcode)
    {
        if (m_proxy_handler)
            m_proxy_handler->on_http_error(sender, errcode);
    }

protected:
    void set_proxy_handler(boost::shared_ptr<http_connection_listener> proxy_handler)
    {
        m_proxy_handler = proxy_handler;
    }
    void set_proxy_handler(http_connection_listener* proxy_handler)
    {
        m_proxy_handler.reset(proxy_handler);
    }
    void reset_proxy_handler()
    {
        m_proxy_handler.reset();
    }
    boost::shared_ptr<http_connection_listener> get_proxy_handler()
    {
        return m_proxy_handler;
    }

private:
    boost::shared_ptr<http_connection_listener> m_proxy_handler;
};


}
