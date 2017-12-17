#pragma once

#include <xul/net/tcp_server.hpp>
#include <xul/net/io_service.hpp>
#include <xul/net/tcp_acceptor.hpp>
#include <xul/net/detail/dummy_io_server_listener.hpp>
#include <xul/xio/net/tcp_session.hpp>
#include <xul/data/tree_map.hpp>
#include <xul/util/log.hpp>
#include <set>
#include <stdint.h>
#include <assert.h>


namespace xul {


class tcp_server_impl
    : public object_impl<tcp_server>
    , public tcp_acceptor_listener
{
public:
    class tcp_server_session : public tcp_session
    {
    public:
        explicit tcp_server_session(tcp_server_impl& owner, tcp_socket* sock)
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
        tcp_server_impl& m_owner;
    };

    typedef boost::intrusive_ptr<tcp_server_session> tcp_server_session_ptr;
    typedef map<tcp_server_session*, tcp_server_session*> tcp_server_session_map;
    //typedef std::map<tcp_server_session*, tcp_server_session_ptr> tcp_server_session_collection;
    //typedef std::set<tcp_server_session_ptr> tcp_server_session_set;

    explicit tcp_server_impl(io_service* ioFactory) : m_io_factory(ioFactory)
    {
        XUL_LOGGER_INIT("tcp_server");
        XUL_DEBUG("new");
        m_listener = NULL;
        m_enable_reuse_address = false;
        m_acceptor = m_io_factory->create_tcp_acceptor();
        m_acceptor->set_listener(this);
        m_sessions = new tree_map<tcp_server_session*, tcp_server_session*>;
    }
    virtual ~tcp_server_impl()
    {
        XUL_DEBUG("delete");
        destroy();
    }

    virtual bool feed_client(tcp_socket* sock)
    {
        return m_acceptor->feed_client(sock);
    }
    virtual void enable_reuse_address(bool enabled)
    {
        m_enable_reuse_address = enabled;
    }

    virtual bool start(int port)
    {
        stop();
        return m_acceptor->open(port, m_enable_reuse_address);
    }

    virtual void stop()
    {
        m_sessions->clear();
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
        XUL_DEBUG("on_acceptor_client " << sockAddr);
        tcp_server_session_ptr session = create_session(newClient, sockAddr);
        if (m_listener->on_session_create(this, session.get()))
        {
            session->attach_listener();
            session->open_session();
            m_sessions->set(session.get(), session.get());
        }
    }

    virtual void on_acceptor_error(tcp_acceptor* acceptor, int errcode) { }

    void on_session_error(tcp_server_session* session, int errcode)
    {
        tcp_server_session* obj = m_sessions->get_ref(session, NULL);
        if (!obj)
            return;
        tcp_server_session_ptr p = obj;
        p->destroy();
        m_sessions->remove(session);
        m_listener->on_session_destroy(this, p.get(), errcode);
        inet_socket_address addr;
        session->get_remote_address(addr);
        XUL_DEBUG("tcp_server::on_session_error " << addr << " " << xul::make_tuple(session, errcode));
    }

    const tcp_server_session_map* get_sessions() const { return m_sessions.get(); }
    //tcp_server_session_collection& get_sessions() { return m_sessions.get(); }

    virtual void set_listener(io_server_listener* listener)
    {
        m_listener = listener ? listener : dummy_listener();
    }
    virtual int get_session_count() const
    {
        return m_sessions->size();
    }

protected:
    virtual tcp_server_session* create_session(tcp_socket* sock, const inet_socket_address& sockaddr)
    {
        return new tcp_server_session(*this, sock);
    }

    static io_server_listener* dummy_listener()
    {
        static detail::dummy_io_server_listener the_listener;
        return &the_listener;
    }

protected:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<io_service> m_io_factory;

    /// tcp acceptor object
    boost::intrusive_ptr<tcp_acceptor> m_acceptor;

    /// client session collection
    boost::intrusive_ptr<tcp_server_session_map> m_sessions;

    io_server_listener* m_listener;
    bool m_enable_reuse_address;
};


}
