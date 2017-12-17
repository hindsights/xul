#pragma once

#include <xul/net/http/http_server_router.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <vector>
#include <utility>


namespace xul {


class simple_http_server_router_impl : public xul::object_impl<simple_http_server_router>
{
public:
    typedef boost::intrusive_ptr<http_server_route_handler> http_server_route_handler_ptr;
    simple_http_server_router_impl()
    {
    }
    virtual url_handler* create_url_handler(http_server* server, url_session* session, const url_request* req)
    {
        std::string path = req->get_uri()->get_path();
        for (int i = 0; i < m_handlers.size(); ++i)
        {
            if (boost::algorithm::starts_with(path, m_handlers[i].first))
            {
                return m_handlers[i].second->create_url_handler();
            }
        }
        return m_not_found_handler ? m_not_found_handler->create_url_handler() : NULL;
    }
    virtual void add(const char* pattern, http_server_route_handler* handler)
    {
        if (pattern == NULL)
        {
            assert(false);
            return;
        }
        m_handlers.push_back(std::make_pair(std::string(pattern), http_server_route_handler_ptr(handler)));
    }
    virtual void set_not_found_handler(http_server_route_handler* handler)
    {
        m_not_found_handler = handler;
    }

private:
    std::vector<std::pair<std::string, http_server_route_handler_ptr> > m_handlers;
    /// customized
    http_server_route_handler_ptr m_not_found_handler;
};

class simple_http_server_route_handler : public xul::object_impl<http_server_route_handler>
{
public:
    explicit simple_http_server_route_handler(boost::function0<url_handler*> f) : m_func(f)
    {
    }
    virtual url_handler* create_url_handler()
    {
        return m_func();
    }

private:
    boost::function0<url_handler*> m_func;
};

simple_http_server_router* create_simple_http_server_router()
{
    return new simple_http_server_router_impl;
}

http_server_route_handler* create_http_server_route_handler(boost::function0<url_handler*> f)
{
    return new simple_http_server_route_handler(f);
}


}
