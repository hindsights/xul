#pragma once

#include <xul/xio/io_server.hpp>
#include <xul/net/io_factory.hpp>
#include <xul/net/tcp_acceptor.hpp>
#include <xul/xio/net/tcp_session.hpp>
#include <xul/util/listenable_mixin.hpp>
#include <xul/util/log.hpp>
#include <xul/data/tuple.hpp>
#include <boost/noncopyable.hpp>
#include <map>
#include <set>
#include <stdint.h>
#include <assert.h>


namespace xul {


class tcp_server
    : public listenable_mixin<object_impl<io_server>, io_server_listener, detail::dummy_io_server_listener>
    , public tcp_acceptor_listener
{
public:
    class tcp_server_session : public tcp_session
    {
    public:
        explicit tcp_server_session(tcp_server& owner, tcp_socket* sock)
            : tcp_session(sock)
            , m_owner(owner)
        {
        }
        virtual ~tcp_server_session()
        {
        }

    protected:
        virtual void on_session_error(int errcode)
        {
            do_get_listener()->on_session_error(this, errcode);
            m_owner.on_session_error(this, errcode);
        }

    private:
        tcp_server& m_owner;
    };

    typedef boost::intrusive_ptr<tcp_server_session> tcp_server_session_ptr;
    typedef std::map<tcp_server_session*, tcp_server_session_ptr> tcp_server_session_collection;
    typedef std::set<tcp_server_session_ptr> tcp_server_session_set;

    explicit tcp_server(io_service* ioFactory) : m_io_factory(ioFactory)
    {
        m_acceptor = m_io_factory->create_tcp_acceptor();
        m_acceptor->set_listener(this);
    }
    virtual ~tcp_server()
    {
        destroy();
    }

    bool start(uint16_t port, bool enableReuseAddress)
    {
        stop();
        return m_acceptor->open(port, enableReuseAddress);
    }

    void stop()
    {
        m_sessions.clear();
        m_acceptor->close();
    }
    void destroy()
    {
        m_acceptor->reset_listener();
        stop();
    }

    tcp_acceptor* get_acceptor() { return m_acceptor.get(); }

    virtual void on_acceptor_client(tcp_acceptor* acceptor, tcp_socket* newClient, const inet_socket_address& sockAddr)
    {
        assert(m_acceptor.get() == acceptor);
        tcp_server_session_ptr session = create_session(newClient, sockAddr);
        if (do_get_listener()->on_session_create(this, session.get()))
        {
            session->attach_listener();
            session->open_session();
            m_sessions[session.get()] = session;
        }
    }

    virtual void on_acceptor_error(tcp_acceptor* acceptor, int errcode) { }

    void on_session_error(tcp_server_session* session, int errcode)
    {
        tcp_server_session_collection::iterator iter = m_sessions.find(session);
        if (iter == m_sessions.end())
            return;
        tcp_server_session_ptr p = iter->second;
        p->destroy();
        m_sessions.erase(iter);
        do_get_listener()->on_session_destroy(this, p.get(), errcode);
        XUL_APP_DEBUG("tcp_server::on_session_error " << session->get_remote_address() << " " << xul::make_tuple(session, errcode));
    }

    const tcp_server_session_collection& get_sessions() const { return m_sessions; }

protected:
    virtual tcp_server_session* create_session(tcp_socket* sock, const inet_socket_address& sockaddr)
    {
        return new tcp_server_session(*this, sock);
    }

protected:
    boost::intrusive_ptr<io_service> m_io_factory;

    /// tcp acceptor object
    boost::intrusive_ptr<tcp_acceptor> m_acceptor;

    /// client session collection
    tcp_server_session_collection m_sessions;
};


}
