#pragma once

#include <xul/xio/net/tcp_session.hpp>
#include <xul/xio/io_session_adapter.hpp>
#include <xul/io/codec/message_decoder.hpp>
#include <xul/net/name_resolver.hpp>
#include <xul/net/io_service.hpp>
#include <xul/net/inet_socket_address.hpp>


namespace xul {


class tcp_client;

class tcp_client
    : public tcp_session
    , public name_resolver_listener
{
public:
    explicit tcp_client(io_service* ioFactory)
        : tcp_session(ioFactory->create_tcp_socket())
        , m_io_factory(ioFactory)
    {
        m_port = 0;
    }
    virtual ~tcp_client()
    {
        if (m_resolver)
        {
            m_resolver->destroy();
        }
        //if (m_message_decoder)
        //    m_message_decoder->set_listener(NULL);
        this->destroy();
    }

    //virtual void pure_test() { }

    virtual void close()
    {
        if (m_resolver)
        {
            m_resolver->destroy();
            m_resolver.reset();
        }
        if (m_socket && m_socket->is_open())
        {
            m_socket->destroy();
            m_socket = m_io_factory->create_tcp_socket();
            m_socket->set_listener(this);
        }
        tcp_session::close();
    }

    void connect(const std::string& host, uint16_t port)
    {
        //m_socket->destroy();
        //close();
        assert(false == is_open());
        //m_socket = m_io_factory->create_tcp_socket();
        //m_socket->set_listener(this);
        attach_listener();
        m_port = port;
        inet_socket_address addr(host.c_str(), port);
        if (addr.get_inet_address().is_valid())
        {
            do_connect(addr);
            return;
        }
        m_resolver = m_io_factory->create_name_resolver();
        m_resolver->set_listener(this);
        m_resolver->async_resolve(host);
    }

    void connect(const xul::inet_socket_address& addr)
    {
        attach_listener();
        do_connect(addr);
    }

    virtual void on_resolver_address(name_resolver* sender, const std::string& name, int errcode, const std::vector<inet4_address>& addrs)
    {
        if (errcode > 0 || addrs.empty())
        {
            do_get_listener()->on_session_error(this, errcode);
            return;
        }
        inet_socket_address addr;
        addr.set_port(m_port);
        addr.set_raw_ip(addrs[0].get_raw_address());
        do_connect(addr);
    }

    virtual void on_socket_connect(xul::tcp_socket* sender)
    {
        open_session();
    }
    virtual void on_socket_connect_failed(xul::tcp_socket* sender, int errcode)
    {
        do_get_listener()->on_session_error(this, errcode);
    }

protected:
    void do_connect(const inet_socket_address& addr)
    {
        m_address = addr;
        do_connect();
    }
    void do_connect()
    {
        close();
        m_socket->connect(m_address);
    }

private:
    boost::intrusive_ptr<io_service> m_io_factory;
    boost::intrusive_ptr<name_resolver> m_resolver;
    //boost::intrusive_ptr<message_decoder> m_message_decoder;
    inet_socket_address m_address;
    uint16_t m_port;
};

}
