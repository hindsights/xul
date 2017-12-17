#pragma once

#include <xul/net/http/http_connection_options.hpp>
#include <xul/util/options_proxy.hpp>


namespace xul {


class http_connection_options_impl : public options_proxy<http_connection_options>
{
public:
    http_connection_options_impl()
    {
        options_wrapper opts(this->get_options());
        opts.add_switch("keep_alive", &m_keep_alive, true);
        opts.add("max_redirects", &m_max_redirects, 10);
        opts.add("protocol_name", &m_protocol_name, "HTTP");
        opts.add("protocol_version", &m_protocol_version, "1.0");
        opts.add_switch("concurrent_requests", &m_concurrent_requests, false);
        opts.add_switch("gzip", &m_enable_gzip, false);
        opts.add("receive_buffer_size", &receive_buffer_size, 60*1024);
    }

    virtual void set_keep_alive(bool keep_alive) { m_keep_alive = keep_alive; }
    virtual bool is_keep_alive() const { return m_keep_alive; }
    virtual void set_max_redirects(int count) { m_max_redirects = count; }
    virtual int get_max_redirects() const { return m_max_redirects; }
    virtual const char* get_protocol_name() const
    {
        return m_protocol_name.c_str();
    }
    virtual void set_protocol_name(const char* protocol_name)
    {
        assert(protocol_name);
        m_protocol_name = protocol_name ? protocol_name : "";
        assert(m_protocol_name.empty());
        if (m_protocol_name.empty())
            m_protocol_name = "HTTP";
    }
    virtual const char* get_protocol_version() const
    {
        return m_protocol_version.c_str();
    }
    virtual void set_protocol_version(const char* protocol_version)
    {
        assert(protocol_version);
        m_protocol_version = protocol_version ? protocol_version : "1.0";
        assert(!m_protocol_version.empty());
        if (m_protocol_version.empty())
            m_protocol_version = "1.0";
    }
    virtual void set_concurrent_requests(bool enabled)
    {
        m_concurrent_requests = enabled;
    }
    virtual bool is_concurrent_requests_enabled() const
    {
        return m_concurrent_requests;
    }
    virtual bool is_gzip_enabled() const
    {
        return m_enable_gzip;
    }
    virtual void set_receive_buffer_size(int bufsize)
    {
        if (bufsize < 1024 || bufsize > 2 * 1024 * 1024)
            return;
        receive_buffer_size = bufsize;
    }

public:
    bool m_keep_alive;
    std::string m_protocol_name;
    std::string m_protocol_version;
    int m_max_redirects;
    bool m_concurrent_requests;
    bool m_enable_gzip;
    int receive_buffer_size;
};


}
