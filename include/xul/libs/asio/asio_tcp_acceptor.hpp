#pragma once

#include <xul/net/tcp_acceptor.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/net/detail/dummy_tcp_acceptor_listener.hpp>
#include <xul/libs/asio/asio_tcp_socket.hpp>
#include <xul/util/listenable_mixin.hpp>
#include <xul/util/intrusive_shared_from_this.hpp>
#include <xul/log/log.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/error.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>


namespace xul {


class asio_tcp_acceptor
    : public closable_listenable_mixin<object_impl<tcp_acceptor>, tcp_acceptor_listener, detail::dummy_tcp_acceptor_listener, detail::checked_tcp_acceptor_listener>
    , public intrusive_shared_from_this<asio_tcp_acceptor>
{
public:
    typedef boost::asio::ip::tcp::acceptor native_type;
    typedef boost::asio::ip::tcp::socket client_socket_type;
    typedef boost::shared_ptr<client_socket_type> client_socket_ptr_type;

    //typedef detail::asio_tcp_acceptor_impl::native_type native_type;
    //typedef detail::asio_tcp_acceptor_impl::client_type client_type;
    //typedef detail::asio_tcp_acceptor_impl::client_ptr_type client_ptr_type;

    explicit asio_tcp_acceptor(boost::asio::io_service& ios)
        : m_native(ios)
    {
        XUL_LOGGER_INIT("tcp_acceptor");
        XUL_REL_INFO("new");
    }
    explicit asio_tcp_acceptor(boost::asio::io_service& ios, int handle)
        : m_native(ios, boost::asio::ip::tcp::v4(), handle)
    {
        XUL_LOGGER_INIT("tcp_acceptor ");
        XUL_REL_INFO("new" << handle);
    }
    virtual ~asio_tcp_acceptor()
    {
        XUL_REL_INFO("delete");
        close();
    }

    const native_type& get_native() const { return m_native; }
    native_type& get_native() { return m_native; }
    const boost::system::error_code& get_last_error() const { return m_last_error; }

    bool is_open() const
    {
        return get_native().is_open();
    }

    bool get_local_address(xul::socket_address& addr) const
    {
        if ( false == is_open() )
            return false;
        return xul::endpoints::to_socket_address( get_native().local_endpoint(m_last_error), addr );
    }
    virtual inet_socket_address get_local_address() const
    {
        inet_socket_address addr;
        get_local_address(addr);
        return addr;
    }
    virtual bool feed_client(tcp_socket* sock)
    {
        inet_socket_address addr;
        if (!sock->get_remote_address(addr))
            return false;
        do_get_listener()->on_acceptor_client(this, sock, addr);
        return true;
    }

    void close()
    {
        get_native().close(m_last_error);
    }

    void enable_reuse_address(bool enable)
    {
        get_native().set_option(boost::asio::ip::tcp::acceptor::reuse_address(enable), m_last_error);
    }

    bool open(u_short port, bool enableReuseAddress)
    {
        get_native().close(m_last_error);
        get_native().open(boost::asio::ip::tcp::v4(), m_last_error);
        if (m_last_error)
        {
            XUL_REL_ERROR("socket open failed " << m_last_error);
            return false;
        }
        if (enableReuseAddress)
        {
            enable_reuse_address(enableReuseAddress);
        }
        boost::asio::ip::tcp::endpoint addr(boost::asio::ip::address_v4(), port);
        get_native().bind(addr, m_last_error);
        if (m_last_error)
        {
            XUL_REL_ERROR("socket bind failed " << m_last_error << " " << port);
            return false;
        }
        get_native().listen(boost::asio::socket_base::max_connections, m_last_error);
        if (m_last_error)
        {
            XUL_REL_ERROR("socket listen failed " << m_last_error << " " << port);
            return false;
        }
        start_accept();
        return true;
    }

    virtual tcp_acceptor_listener* get_listener() const
    {
        return do_get_listener();
    }
private:
    void on_accept(const boost::system::error_code& err, const client_socket_ptr_type& client)
    {
        if (err)
        {
            int errcode = err.value();
            if (errcode != boost::asio::error::invalid_argument && errcode != boost::asio::error::bad_descriptor)
            {
                start_accept();
            }
            else
            {
                XUL_REL_ERROR("on_accept abort " << errcode << " " << m_native.native());
            }
            //printf("tcp_acceptor::on_accept failed %d\n", err.value());
            do_get_listener()->on_acceptor_error(this, errcode);
        }
        else
        {
            start_accept();
            boost::intrusive_ptr<tcp_socket> sock = new asio_tcp_socket(client);
            inet_socket_address addr;
            sock->get_remote_address(addr);
            do_get_listener()->on_acceptor_client(this, sock.get(), addr );
        }
    }

    void start_accept()
    {
        if (false == m_native.is_open())
        {
            //printf("acceptor is closed %p\n", this);
            return;
        }
        m_new_client.reset(new client_socket_type(m_native.get_io_service()));
        m_native.async_accept(*m_new_client, boost::bind(&asio_tcp_acceptor::on_accept, this->shared_from_this(), boost::asio::placeholders::error));
    }

    void on_accept(const boost::system::error_code& err)
    {
        client_socket_ptr_type client = m_new_client;
        on_accept(err, client);
    }

private:
    XUL_LOGGER_DEFINE();
    mutable boost::system::error_code m_last_error;
    native_type m_native;
    client_socket_ptr_type m_new_client;
};


}
