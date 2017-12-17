#pragma once

#include <xul/net/http/background_http_requester.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/data/string_table.hpp>
#include <xul/log/log.hpp>
#include <boost/bind.hpp>


namespace xul { namespace detail {


class background_http_requester_impl : public object_impl<background_http_requester>
{
public:
    typedef boost::intrusive_ptr<http_client> http_client_ptr;
    typedef std::map<http_client*, http_client_ptr> http_client_collection;
    
    explicit background_http_requester_impl(io_service* ios) : m_ios(ios)
    {
        m_headers = create_istring_map();
    }
    
    virtual void set_common_header(const char* key, const char* val)
    {
        m_headers->set(key, val);
    }
    //virtual xul::http_client* request(url_request* req, const char* body, int bodysize)
    //{
    //    return NULL;
    //}
    virtual xul::http_client* post(const char* url, const std::string& body, const std::string& content_type)
    {
        xul::http_client* client = create_client();
        boost::intrusive_ptr<uri> u = xul::create_uri();
        if (!u->parse(url))
            return NULL;
        client->async_post_data(u.get(), 4096,
                                boost::bind(&background_http_requester_impl::on_callback, this, _1, _2, _3, _4, _5, _6),
                                body, content_type,
                                m_headers.get());
        return client;
    }
    virtual xul::http_client* notify(const char* url)
    {
        xul::http_client* client = create_client();
        client->async_download_data(url, 4096, boost::bind(&background_http_requester_impl::on_callback, this, _1, _2, _3, _4, _5, _6), m_headers.get());
        return client;
    }

    void on_callback(xul::http_client* sender, uri* url, int errcode, url_response* resp, uint8_t* data, size_t size)
    {
        XUL_DEBUG("on_callback " << errcode << " " << m_clients.size() << " " << sender);
        m_clients.erase(sender);
    }

    xul::http_client* create_client()
    {
        boost::intrusive_ptr<xul::http_client> client = new xul::http_client(m_ios.get());
        assert(m_clients.find(client.get()) == m_clients.end());
        m_clients[client.get()] = client;
        return client.get();
    }

private:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<io_service> m_ios;
    http_client_collection m_clients;
    boost::intrusive_ptr<string_table> m_headers;
};


} }
