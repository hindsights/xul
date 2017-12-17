#pragma once

#include <xul/net/tcp_socket.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/net/detail/dummy_tcp_socket_listener.hpp>
#include <xul/util/listenable_mixin.hpp>
#include <xul/net/asio/endpoints.hpp>
#include <xul/macro/memory.hpp>
#include <xul/data/buffer.hpp>
#include <xul/util/listenable.hpp>
#include <xul/log/log.hpp>
#include <xul/os/thread.hpp>
#include <xul/data/bit_converter.hpp>
#include <xul/data/tuple.hpp>
#include <xul/util/intrusive_shared_from_this.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/error.hpp>
#include <boost/thread/thread.hpp>

#include <boost/detail/lightweight_mutex.hpp>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include <list>
#include <ostream>


namespace xul {


class asio_tcp_socket
    : public closable_listenable_mixin<object_impl<tcp_socket>, tcp_socket_listener, detail::dummy_tcp_socket_listener, detail::checked_tcp_socket_listener>
    , public intrusive_shared_from_this<asio_tcp_socket>
{
public:
    typedef boost::asio::ip::tcp::socket native_type;
    typedef boost::shared_ptr<native_type> native_ptr_type;

    typedef boost::shared_ptr<xul::byte_buffer> byte_buffer_ptr;

    typedef boost::detail::lightweight_mutex lock_type;

    typedef boost::shared_ptr<tcp_socket> ptr_type;

    typedef std::list<byte_buffer_ptr> sending_queue_type;

    typedef boost::asio::ip::tcp::socket impl_type;
    typedef boost::shared_ptr<impl_type> impl_ptr_type;

    explicit asio_tcp_socket(native_ptr_type native)
        : m_connected(true)
        , m_native(native)
    {
        xul::inet_socket_address addr;
        assert(get_remote_address(addr));
        do_init();
        // save thread id, this function is usually invoked in on_acceptor_client
        save_thread_id();
        force_close_immediately();
    }

    explicit asio_tcp_socket(boost::asio::io_service& ios)
        : m_connected(false)
        , m_native(new native_type(ios))
    {
        do_init();
    }

    explicit asio_tcp_socket(boost::asio::io_service& ios, int handle)
        : m_connected(false)
        , m_native(new native_type(ios, boost::asio::ip::tcp::v4(), handle))
    {
        do_init();
    }

    virtual ~asio_tcp_socket()
    {
        XUL_DEBUG("delete");
        close();
    }
    virtual bool is_connected() const
    {
        return m_connected;
    }

    const native_type& get_native() const { return *m_native; }
    native_type& get_native() { return *m_native; }
    native_ptr_type& get_native_ptr() { return m_native; }

    virtual int get_handle() const
    {
        return m_native->native();
    }

    bool set_linger(bool enabled, int timeout)
    {
        if (!m_native || !m_native->is_open())
            return false;
        native_type::linger opt;
        opt.enabled(enabled);
        opt.timeout(timeout);
        m_native->set_option(opt, m_last_error);
        return !m_last_error;
    }
    bool force_close_immediately()
    {
#if 0
        //set_linger(true, 0);
        native_type::linger opt;
        m_native->get_option(opt, m_last_error);
        XUL_DEBUG("linger option: " << make_tuple(opt.enabled(), opt.timeout()));
#endif
        return false;
    }

    int get_writing_queue_size() const
    {
        return get_sending_queue_size();
    }
    int get_max_writing_queue_size() const
    {
        return get_max_sending_queue_size();
    }

    void set_max_writing_queue_size(int maxSendingQueueSize)
    {
        set_max_sending_queue_size(maxSendingQueueSize);
    }

    int get_sending_queue_size() const
    {
        return m_sending_queue.size();
    }
    int get_max_sending_queue_size() const
    {
        return m_max_sending_queue_size;
    }

    void set_max_sending_queue_size(int maxSendingQueueSize)
    {
        if (0 == maxSendingQueueSize)
        {
            XUL_ERROR("invalid max_sending_queue_size");
            return;
        }
        m_max_sending_queue_size = maxSendingQueueSize;
    }

    bool get_local_address(xul::socket_address& addr) const
    {
        if ( false == is_open() )
            return false;
        return xul::endpoints::to_socket_address( get_native().local_endpoint(m_last_error), addr );
    }

    bool get_remote_address(xul::socket_address& addr) const
    {
        if ( false == is_open() )
            return false;
        return xul::endpoints::to_socket_address( get_native().remote_endpoint(m_last_error), addr );
    }

    virtual inet_socket_address get_local_address() const
    {
        inet_socket_address addr;
        return get_local_address(addr) ? addr : inet_socket_address();
    }
    virtual inet_socket_address get_remote_address() const
    {
        inet_socket_address addr;
        return get_remote_address(addr) ? addr : inet_socket_address();
    }

    bool is_open() const
    {
        return get_native().is_open();
    }

    virtual bool bind(const socket_address& addr)
    {
        boost::asio::ip::tcp::endpoint ep = endpoints::to_tcp_endpoint(addr);
        get_native().bind(ep, m_last_error);
        return !m_last_error;
    }
    virtual bool bind_port(int port)
    {
        xul::inet_socket_address addr;
        addr.set_ip((uint32_t)0);
        addr.set_port(port);
        return bind(addr);
    }

    void close()
    {
        if (is_open())
        {
            XUL_DEBUG("close " << m_native->native() << " " << do_get_listener());
            get_native().shutdown(boost::asio::socket_base::shutdown_both, m_last_error);
            get_native().close(m_last_error);
            {
                //lock_type::scoped_lock lock(m_lock);
                //m_sending_queue.clear();
            }
        }
        // close may be invoked from other threads
        m_connected = false;
    }

    virtual void destroy()
    {
        XUL_DEBUG("destroy " << xul::make_tuple(m_native->native(), do_get_listener()));
        reset_listener();
        close();
    }

    bool connect( const char* ip, u_short port )
    {
        xul::inet_socket_address sockAddr(ip, port);
        if (sockAddr.get_inet_address().is_none() || 0 == port)
        {
            assert(false);
            return false;
        }
        return connect(sockAddr);
    }
    bool connect(const socket_address& sockAddr)
    {
        boost::asio::ip::tcp::endpoint addr = xul::endpoints::to_tcp_endpoint(sockAddr);
        return connect(addr);
    }

    bool send(const void* data, size_t size)
    {
        return send_n(data, size);
    }

    bool send_n(const void* data, size_t size)
    {
        assert(data != NULL && size > 0);
        if (m_sending_queue.size() >= m_max_sending_queue_size)
        {
            XUL_WARN("sending queue is too large " << xul::make_tuple( m_sending_queue.size(), m_max_sending_queue_size ));
            return false;
        }
        byte_buffer_ptr buf(new byte_buffer(size));
        memcpy(buf->data(), data, size);
        m_sending_queue.push_back(buf);
        // if sending_queue has only this new buffer, send it immediately
        if (m_sending_queue.size() == 1)
        {
            process_sending_queue();
        }
        return true;
    }

    void receive(size_t size)
    {
        assert(false == m_receiving);
        assert(size > 0);
        byte_buffer_ptr buf = create_receive_buffer();
        buf->ensure_size(size);
        m_receiving = true;
        do_receive(buf);
    }

    void receive_n(size_t size)
    {
        assert(false == m_receiving);
        assert(size > 0);
        byte_buffer_ptr buf = create_receive_buffer();
        buf->ensure_size(size);
        m_receiving = true;
        do_receive_n(buf);
    }

    virtual bool is_sending() const { return !m_sending_queue.empty(); }
    virtual bool is_receiving() const { return m_receiving; }

    virtual tcp_socket_listener* get_listener() const
    {
        return do_get_listener();
    }
private:
    byte_buffer_ptr create_receive_buffer()
    {
        //if (m_free_receive_buffers.empty())
            return byte_buffer_ptr(new byte_buffer);
#if 0
        byte_buffer_ptr buf = m_free_receive_buffers.front();
        m_free_receive_buffers.pop_front();
        return buf;
#endif
    }
    void free_receive_buffer(byte_buffer_ptr buf)
    {
        //m_free_receive_buffers.push_back(buf);
    }
    bool connect(const boost::asio::ip::tcp::endpoint& addr)
    {
        //assert(!is_open());
        //bool is_opened = is_open();
        this->close();
        m_remote_address = addr;
        //if (!is_opened)
        do_connect();
        return true;
    }
    void on_receive(const boost::system::error_code& errcode, size_t bytes, const byte_buffer_ptr& buf)
    {
        //save_thread_id();
        //printf("on_receive %p %d\n", this, bytes);
        m_receiving = false;
        if (!m_connected)
        {
            XUL_WARN("on_receive when connection lost or closed " << xul::make_tuple(errcode, bytes));
            //return;
        }
        if (errcode)
        {
            if (m_sending_queue.size() > 0)
            {
                XUL_REL_WARN("on_socket_receive_failed abort sending " << xul::make_tuple(errcode, m_sending_queue.size()));
            }
            m_connected = false;
            m_sending_queue.clear();
            XUL_DEBUG("on_socket_receive_failed " << errcode << " " << this);
            do_get_listener()->on_socket_receive_failed(this, errcode.value());
            return;
        }
        //assert(bytes == m_packet_body.size());
        XUL_DEBUG("on_socket_receive " << bytes << " " << this);
        do_get_listener()->on_socket_receive(this, buf->data(), bytes);
    }
    void on_send(const boost::system::error_code& errcode, size_t bytes, const byte_buffer_ptr& bufReal)
    {
        //XUL_DEBUG("tcp_socket::on_send " << xul::make_tuple(xul::thread::get_current_thread_id(), m_sending_queue.size(), bytes, errcode));
        //save_thread_id();
        if (!m_connected)
        {
            XUL_DEBUG("on_send when connection lost or closed " << xul::make_tuple(errcode, bytes));
            //return;
        }
        if (errcode)
        {
            m_connected = false;
            // clear sending queue on error
            {
                //lock_type::scoped_lock lock(m_lock);
                m_sending_queue.clear();
            }
            XUL_DEBUG("on_socket_send_failed " << xul::make_tuple(errcode, bytes, m_sending_queue.size()));
            do_get_listener()->on_socket_send_failed(this, errcode.value());
            return;
        }
        XUL_DEBUG("on_socket_send " << xul::make_tuple(errcode, m_sending_queue.size()));
        {
            //lock_type::scoped_lock lock(m_lock);
            if (m_sending_queue.empty())
            {
                // maybe the socket is closed
                //assert(errcode);
                XUL_ERROR("on_send: sending_queue is empty");
                return;
            }
            m_sending_queue.pop_front();
            // continue process sending queue
            process_sending_queue();
        }
        do_get_listener()->on_socket_send(this, bytes);
    }

    void on_connect( const boost::system::error_code& errcode )
    {
        XUL_DEBUG("socket.on_connect " << xul::make_tuple(errcode.value(), m_connected));
        save_thread_id();
        {
            //lock_type::scoped_lock lock(m_lock);
            m_sending_queue.clear();
        }
        assert(!m_connected);
        m_connected = !errcode;
        m_receiving = false;
        if (errcode)
        {
            do_get_listener()->on_socket_connect_failed(this, errcode.value());
        }
        else
        {
            do_get_listener()->on_socket_connect(this);
        }
    }

    void process_sending_queue()
    {
        if (m_sending_queue.empty())
            return;
        // if socket is closed, clear sending queue
        if (false == get_native().is_open())
        {
            m_sending_queue.clear();
            XUL_ERROR("tcp_socket::process_sending_queue: socket is closed");
            return;
        }
        assert(boost::this_thread::get_id() == m_io_service_thread || boost::thread::id() == m_io_service_thread);
        const byte_buffer_ptr& buf = m_sending_queue.front();
        do_send_n(buf);
    }

    void do_connect(const boost::asio::ip::tcp::endpoint& addr)
    {
        XUL_DEBUG("do_connect " << addr);
        assert(!is_open());
        m_native->async_connect(
            addr,
            boost::bind(
            &asio_tcp_socket::on_connect,
            this->shared_from_this(),
            boost::asio::placeholders::error
            )
            );
        force_close_immediately();
    }
    void do_connect()
    {
        do_connect(m_remote_address);
    }

    void do_init()
    {
        XUL_LOGGER_INIT("asio_tcp_socket");
        XUL_DEBUG("new");
        assert(m_native);
        //m_input_buffer.reset(new byte_buffer);
        m_max_sending_queue_size = 16;
        m_receiving = false;
    }

    void save_thread_id()
    {
#if 1
        if (m_io_service_thread != boost::thread::id())
            return;
        m_io_service_thread = boost::this_thread::get_id();
#endif
    }

    void do_receive(const byte_buffer_ptr& buf)
    {
        assert(buf->size() > 0);
        assert(m_connected);
        //printf("do_receive %p %d\n", this, buf->size());
        XUL_DEBUG("do_receive " << buf->size());
        m_native->async_receive(
            boost::asio::buffer( buf->data(), buf->size() ),
            boost::bind(
            &asio_tcp_socket::on_receive,
            this->shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            buf
            )
            );
    }
    void do_receive_n(const byte_buffer_ptr& buf)
    {
        assert(buf->size() > 0);
        XUL_DEBUG("do_receive_n " << buf->size());
        boost::asio::async_read(
            *m_native,
            boost::asio::buffer( buf->data(), buf->size() ),
            boost::asio::transfer_at_least(buf->size()),
            boost::bind(
            &asio_tcp_socket::on_receive,
            this->shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            buf
            )
            );
    }
    void do_send_n(const byte_buffer_ptr& buf)
    {
        assert(buf->size() > 0);
        boost::asio::async_write(
            *m_native,
            boost::asio::buffer( buf->data(), buf->size() ),
            boost::asio::transfer_at_least(buf->size()),
            boost::bind(
            &asio_tcp_socket::on_send,
            this->shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            buf
            )
            );
    }

private:
    XUL_LOGGER_DEFINE();
    native_ptr_type m_native;
    boost::asio::ip::tcp::endpoint m_remote_address;
    mutable boost::system::error_code m_last_error;

    //lock_type m_lock;

    boost::thread::id m_io_service_thread;

    sending_queue_type m_sending_queue;
    int m_max_sending_queue_size;

    bool m_connected;
    bool m_receiving;
    //std::list<byte_buffer_ptr> m_free_receive_buffers;
};


}
