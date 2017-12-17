#pragma once

#include <xul/net/udp_socket.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/net/detail/dummy_udp_socket_listener.hpp>
#include <xul/net/asio/endpoints.hpp>
#include <xul/data/buffer.hpp>
#include <xul/util/listenable.hpp>
#include <xul/util/intrusive_shared_from_this.hpp>
#include <xul/data/printables.hpp>
#include <xul/log/log.hpp>

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/error.hpp>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>


namespace xul {


class asio_udp_socket
    : public object_impl<udp_socket>
    , public intrusive_shared_from_this<asio_udp_socket>
{
public:
    typedef boost::asio::ip::udp::socket native_type;

    struct udp_receive_session
    {
        xul::byte_buffer buffer;
        boost::asio::ip::udp::endpoint remote_endpoint;
    };
    typedef boost::shared_ptr<udp_receive_session> udp_receive_session_ptr;

    explicit asio_udp_socket(boost::asio::io_service& ios) : m_native(ios)
    {
        XUL_LOGGER_INIT("asio_udp_socket");
        XUL_DEBUG("new");
        m_checked_listener.reset(new detail::checked_udp_socket_listener);
        m_dummy_listener.reset(new detail::dummy_udp_socket_listener);
        set_listener(NULL);
    }

    explicit asio_udp_socket(boost::asio::io_service& ios, int handle) : m_native(ios, boost::asio::ip::udp::v4(), handle)
    {
        XUL_LOGGER_INIT("asio_udp_socket");
        XUL_DEBUG("new");
        m_checked_listener.reset(new detail::checked_udp_socket_listener);
        m_dummy_listener.reset(new detail::dummy_udp_socket_listener);
        set_listener(NULL);
    }

    virtual ~asio_udp_socket()
    {
        XUL_DEBUG("delete");
        this->close();
    }

    native_type& get_native() { return m_native; }
    const native_type& get_native() const { return m_native; }

    virtual void set_listener(udp_socket_listener* listener)
    {
        m_listener = listener ? listener : m_dummy_listener.get();
    }

    bool is_open() const
    {
        return get_native().is_open();
    }

    void close()
    {
        //m_impl->get_socket().cancel(m_last_error);
        XUL_DEBUG("close");
        get_native().close(m_last_error);
        assert( !m_last_error );
    }
    void destroy()
    {
        set_listener(NULL);
        close();
    }

    bool get_local_address(xul::socket_address* addr) const
    {
        if ( false == is_open() )
            return false;
        return xul::endpoints::to_socket_address( get_native().local_endpoint(m_last_error), *addr );
    }

    virtual xul::inet_socket_address get_local_address() const
    {
        inet_socket_address addr;
        return get_local_address(&addr) ? addr : inet_socket_address();
    }

    bool open( u_short port )
    {
        this->close();

        boost::asio::ip::udp::endpoint ep( boost::asio::ip::udp::v4(), port );

        get_native().open( boost::asio::ip::udp::v4(), m_last_error );
        if ( m_last_error )
            return false;
        get_native().bind( ep, m_last_error );
        if ( m_last_error )
            return false;
        boost::asio::detail::io_control::non_blocking_io iocmd(true);
        get_native().io_control(iocmd, m_last_error);
        return true;
    }

    void receive( size_t maxSize )
    {
        udp_receive_session_ptr session(new udp_receive_session);
        session->buffer.resize(maxSize);
        do_receive(session);
    }

    bool send(const void* data, size_t size, const xul::inet_socket_address& addr)
    {
        //XUL_DEBUG("send " << size << " to " << addr);
        boost::asio::ip::udp::endpoint ep = xul::endpoints::to_udp_endpoint( addr );
        get_native().send_to(
            boost::asio::buffer(data, size),
            ep,
            0,
            m_last_error
            );

        if ( !m_last_error )
            return true;

#if defined(BOOST_WINDOWS)
        if ( boost::asio::error::would_block == m_last_error || boost::asio::error::in_progress == m_last_error )
#else
        if ( boost::asio::error::try_again == m_last_error )
#endif
        {
            return true;
        }
        if (is_open())
        {
            XUL_REL_ERROR("send failed " << xul::make_tuple(m_native.native(), size, m_last_error) << " " << addr);
            //assert(false);
        }
        XUL_DEBUG("send failed 1: " <<  xul::make_tuple(m_native.native(), size, m_last_error) << " " << addr);
        return false;
    }

private:
    udp_socket_listener* do_get_listener()
    {
        return m_listener;
    }
    void on_receive(const boost::system::error_code& err, size_t bytes, const udp_receive_session_ptr& session)
    {
        XUL_DEBUG("on_receive " << xul::make_tuple(err.value(), bytes));
        //APP_DBG_DEBUG("udp_socket::on_receive " << err.value());
        //        TRACE("TCPSocketImpl::HandleLengthEvent %p %d %d\n", this, m_closed, m_test);
        if (err)
        {
            do_get_listener()->on_socket_receive_failed( this, err.value() );
        }
        else
        {
            assert( bytes > 0 );
            xul::inet_socket_address addr = xul::endpoints::to_inet_socket_address( session->remote_endpoint );
            do_get_listener()->on_socket_receive(this, session->buffer.data(), bytes, &addr );
        }
    }

    void do_receive(const udp_receive_session_ptr& session)
    {
        XUL_DEBUG("do_receive " << session->buffer.size());
        assert(session);
        assert(session->buffer.size() > 0);
        m_native.async_receive_from(
            boost::asio::buffer(session->buffer.data(), session->buffer.size()),
            session->remote_endpoint,
            boost::bind(
            &asio_udp_socket::on_receive,
            this->shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            session
            )
            );
    }

private:
    XUL_LOGGER_DEFINE();
    boost::asio::ip::udp::socket m_native;
    mutable boost::system::error_code m_last_error;
    udp_socket_listener* m_listener;
    boost::shared_ptr<udp_socket_listener> m_checked_listener;
    boost::shared_ptr<udp_socket_listener> m_dummy_listener;
};


}
