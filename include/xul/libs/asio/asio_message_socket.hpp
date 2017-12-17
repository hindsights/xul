#pragma once

#include <xul/net/message_socket.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/net/detail/dummy_message_socket_listener.hpp>
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

#include <boost/detail/lightweight_mutex.hpp>

#include <boost/asio/detail/handler_tracking.hpp>
#include <boost/asio/detail/config.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/fenced_block.hpp>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include <list>
#include <ostream>

#include <sys/types.h>
#include <sys/socket.h>


namespace xul {


namespace detail {


class socket_message_data
{
public:
    socket_message_data()
    {
        clear();
    }

    struct ::msghdr* get_header() { return &m_header; }
    const struct ::msghdr* get_header() const { return &m_header; }

    void clear()
    {
        m_io_vectors[0].iov_base = NULL;
        m_io_vectors[0].iov_len = 0;
        memset(&m_header, 0, sizeof(m_header));
    }

    void init_buffer(size_t size, size_t ancillary_data_len, size_t addr_len)
    {
        assert(size > 0);
        buffer.resize(size);
        m_address_buffer.resize(ancillary_data_len);
        m_ancillary_data_buffer.resize(addr_len);
        m_io_vectors[0].iov_base = buffer.data();
        m_io_vectors[0].iov_len = buffer.size();
        m_header.msg_iov = m_io_vectors;
        m_header.msg_iovlen = 1;
        if (m_address_buffer.empty())
        {
            m_header.msg_name = NULL;
            m_header.msg_namelen = 0;
        }
        else
        {
            m_header.msg_name = m_address_buffer.data();
            m_header.msg_namelen = m_address_buffer.size();
        }
        if (m_ancillary_data_buffer.empty())
        {
            m_header.msg_control = NULL;
            m_header.msg_controllen = 0;
        }
        else
        {
            m_header.msg_control = m_ancillary_data_buffer.data();
            m_header.msg_controllen = m_ancillary_data_buffer.size();
        }
    }

    byte_buffer buffer;
private:
    struct ::msghdr m_header;
    byte_buffer m_ancillary_data_buffer;
    byte_buffer m_address_buffer;
    struct iovec m_io_vectors[1];
};

typedef boost::shared_ptr<socket_message_data> socket_message_data_ptr;


class raw_reactive_socket_recvmsg_op_base : public boost::asio::detail::reactor_op
{
public:
    raw_reactive_socket_recvmsg_op_base(int socket,
        const socket_message_data_ptr& data, int in_flags, int& out_flags, func_type complete_func)
        : boost::asio::detail::reactor_op(&raw_reactive_socket_recvmsg_op_base::do_perform, complete_func),
        socket_(socket),
        data_(data),
        in_flags_(in_flags)
    {
    }

    static bool do_perform(reactor_op* base)
    {
        raw_reactive_socket_recvmsg_op_base* o(static_cast<raw_reactive_socket_recvmsg_op_base*>(base));

        if (do_non_blocking_recvmsg(o->socket_, o->data_->get_header(), o->in_flags_, o->ec_, o->bytes_transferred_))
            return true;
        return false;
    }
    static int invoke_recvmsg(int s, struct ::msghdr* msg, int flags, boost::system::error_code& ec)
    {
        int result = boost::asio::detail::socket_ops::error_wrapper(::recvmsg(s, msg, flags), ec);
        if (result >= 0)
        {
            ec = boost::system::error_code();
        }
        return result;
    }
    static bool do_non_blocking_recvmsg(int s, struct ::msghdr* msg, int flags, boost::system::error_code& ec, size_t& bytes_transferred)
    {
        for (;;)
        {
            // Read some data.
            int bytes = invoke_recvmsg(s, msg, flags, ec);

            // Retry operation if interrupted by signal.
            if (ec == boost::asio::error::interrupted)
                continue;

            // Check if we need to run the operation again.
            if (ec == boost::asio::error::would_block
                || ec == boost::asio::error::try_again)
                return false;

            // Operation is complete.
            if (bytes >= 0)
            {
                ec = boost::system::error_code();
                bytes_transferred = bytes;
            }
            else
                bytes_transferred = 0;

            return true;
        }
    }


private:
    int socket_;
    socket_message_data_ptr data_;
    int in_flags_;
};

template <typename Handler>
class raw_reactive_socket_recvmsg_op :
    public raw_reactive_socket_recvmsg_op_base
{
public:
    BOOST_ASIO_DEFINE_HANDLER_PTR(raw_reactive_socket_recvmsg_op);

    raw_reactive_socket_recvmsg_op(int socket, const socket_message_data_ptr& data, int in_flags, Handler& handler)
        : raw_reactive_socket_recvmsg_op_base(socket, data, in_flags, &raw_reactive_socket_recvmsg_op::do_complete),
        handler_(BOOST_ASIO_MOVE_CAST(Handler)(handler))
    {
    }

    static void do_complete(boost::asio::detail::io_service_impl* owner, boost::asio::detail::operation* base,
        const boost::system::error_code& /*ec*/,
        std::size_t /*bytes_transferred*/)
    {
        // Take ownership of the handler object.
        raw_reactive_socket_recvmsg_op* o(static_cast<raw_reactive_socket_recvmsg_op*>(base));
        ptr p = { boost::addressof(o->handler_), o, o };

        BOOST_ASIO_HANDLER_COMPLETION((o));

        // Make a copy of the handler so that the memory can be deallocated before
        // the upcall is made. Even if we're not about to make an upcall, a
        // sub-object of the handler may be the true owner of the memory associated
        // with the handler. Consequently, a local copy of the handler is required
        // to ensure that any owning sub-object remains valid until after we have
        // deallocated the memory here.
        boost::asio::detail::binder2<Handler, boost::system::error_code, std::size_t>
            handler(o->handler_, o->ec_, o->bytes_transferred_);
        p.h = boost::addressof(handler.handler_);
        p.reset();

        // Make the upcall if required.
        if (owner)
        {
            boost::asio::detail::fenced_block b(boost::asio::detail::fenced_block::half);
            BOOST_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_, handler.arg2_));
            boost_asio_handler_invoke_helpers::invoke(handler, handler.handler_);
            BOOST_ASIO_HANDLER_INVOCATION_END;
        }
    }

private:
    Handler handler_;
};

}

class asio_message_socket
    : public closable_listenable_mixin<object_impl<message_socket>, message_socket_listener, detail::dummy_message_socket_listener, detail::checked_message_socket_listener>
    , public intrusive_shared_from_this<asio_message_socket>
{
public:
    typedef boost::asio::posix::stream_descriptor native_type;
    typedef boost::shared_ptr<native_type> native_ptr_type;

    typedef boost::shared_ptr<detail::socket_message_data> message_data_ptr;

    typedef boost::detail::lightweight_mutex lock_type;

    typedef boost::shared_ptr<message_socket> ptr_type;

    typedef std::list<message_data_ptr> sending_queue_type;

    typedef boost::asio::ip::tcp::socket impl_type;
    typedef boost::shared_ptr<impl_type> impl_ptr_type;

    explicit asio_message_socket(native_ptr_type native)
        : m_connected(true)
        , m_native(native)
    {
        do_init();
        save_thread_id();
        force_close_immediately();
    }

    explicit asio_message_socket(boost::asio::io_service& ios)
        : m_connected(false)
        , m_native(new native_type(ios))
    {
        do_init();
    }

    explicit asio_message_socket(boost::asio::io_service& ios, int handle)
        : m_connected(false)
        , m_native(new native_type(ios, handle))
    {
        do_init();
    }

    virtual ~asio_message_socket()
    {
        XUL_DEBUG("delete");
        close();
    }
    virtual bool is_connected() const
    {
        return m_connected;
    }
    virtual void receive(size_t size, size_t ancillary_data_len, int flags)
    {
    }
    virtual void receive_from(size_t size, size_t ancillary_data_len, size_t addr_len, int flags)
    {
    }

    const native_type& get_native() const { return *m_native; }
    native_type& get_native() { return *m_native; }
    native_ptr_type& get_native_ptr() { return m_native; }

    virtual int get_handle() const
    {
        return m_native->native();
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

    bool is_open() const
    {
        return get_native().is_open();
    }

    void close()
    {
        if (is_open())
        {
            XUL_DEBUG("close " << m_native->native());
            //get_native().shutdown(boost::asio::socket_base::shutdown_both, m_last_error);
            get_native().close(m_last_error);
            {
                //lock_type::scoped_lock lock(m_lock);
                //m_sending_queue.clear();
            }
        }
        m_connected = false;
    }

    virtual void destroy()
    {
        XUL_DEBUG("destroy " << xul::make_tuple(m_native->native(), do_get_listener()));
        reset_listener();
        close();
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
            XUL_ERROR("sending queue is too large " << xul::make_tuple( m_sending_queue.size(), m_max_sending_queue_size ));
            return false;
        }
        message_data_ptr buf(new detail::socket_message_data);
        memcpy(buf->buffer.data(), data, size);
        m_sending_queue.push_back(buf);
        if (m_sending_queue.size() == 1)
        {
            process_sending_queue();
        }
        return true;
    }

    void do_receive_message(size_t size, size_t ancillary_data_len, size_t addr_len, int flags)
    {
        assert(false == m_receiving);
        assert(size > 0);
        message_data_ptr buf = create_receive_buffer();
        buf->init_buffer(size, ancillary_data_len, addr_len);
        m_receiving = true;
        do_receive(buf);
    }

    virtual bool is_sending() const { return !m_sending_queue.empty(); }
    virtual bool is_receiving() const { return m_receiving; }

    virtual message_socket_listener* get_listener() const
    {
        return do_get_listener();
    }
private:
    message_data_ptr create_receive_buffer()
    {
        if (m_free_receive_buffers.empty())
            return message_data_ptr(new detail::socket_message_data);
        message_data_ptr buf = m_free_receive_buffers.front();
        m_free_receive_buffers.pop_front();
        return buf;
    }
    void free_receive_buffer(message_data_ptr buf)
    {
        m_free_receive_buffers.push_back(buf);
    }
    void on_receive(const boost::system::error_code& errcode, size_t bytes, const message_data_ptr& buf)
    {
        //save_thread_id();
        //printf("on_receive %p %d\n", this, bytes);
        m_receiving = false;
        m_connected = !errcode;
        //if (errcode && errcode.value() != boost::asio::error::eof)
        if (errcode)
        {
            XUL_DEBUG("on_socket_receive_failed " << errcode << " " << this);
            do_get_listener()->on_socket_receive_failed(this, errcode.value());
            free_receive_buffer(buf);
            return;
        }
        //assert(bytes == m_packet_body.size());
        XUL_DEBUG("on_socket_receive " << bytes << " " << this);
        //do_get_listener()->on_socket_receive(this, buf->buffer.data(), bytes);
        free_receive_buffer(buf);
    }
    void on_send(const boost::system::error_code& errcode, size_t bytes, const message_data_ptr& bufReal)
    {
        //XUL_DEBUG("message_socket::on_send " << xul::make_tuple(xul::thread::get_current_thread_id(), m_sending_queue.size(), bytes, errcode));
        //save_thread_id();
        m_connected = !errcode;
        if (errcode)
        {
            {
                //lock_type::scoped_lock lock(m_lock);
                m_sending_queue.clear();
            }
            do_get_listener()->on_socket_send_failed(this, errcode.value());
            return;
        }
        {
            //lock_type::scoped_lock lock(m_lock);
            if (m_sending_queue.empty())
            {
                //assert(errcode);
                XUL_ERROR("on_send: sending_queue is empty");
                return;
            }
            m_sending_queue.pop_front();
            process_sending_queue();
        }
        do_get_listener()->on_socket_send(this, bytes);
    }

    void process_sending_queue()
    {
        if (m_sending_queue.empty())
            return;
        if (false == get_native().is_open())
        {
            m_sending_queue.clear();
            XUL_ERROR("message_socket::process_sending_queue: socket is closed");
            return;
        }
        assert(boost::this_thread::get_id() == m_io_service_thread || boost::thread::id() == m_io_service_thread);
        const message_data_ptr& buf = m_sending_queue.front();
        do_send_n(buf);
    }

    void do_init()
    {
        XUL_LOGGER_INIT("asio_message_socket");
        XUL_DEBUG("new");
        assert(m_native);
        //m_input_buffer.reset(new message_data);
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

    void do_receive(const message_data_ptr& buf)
    {
#if 0
        assert(buf->size() > 0);
        //printf("do_receive %p %d\n", this, buf->size());
        m_native->async_receive(
            boost::asio::buffer( buf->buffer.data(), buf->buffer.size() ),
            boost::bind(
            &asio_message_socket::on_receive,
            this->shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            buf
            )
            );
#endif
    }
    void do_send_n(const message_data_ptr& buf)
    {
#if 0
        assert(buf->size() > 0);
        boost::asio::async_write(
            *m_native,
            boost::asio::buffer( buf->buffer.data(), buf->buffer.size() ),
            boost::asio::transfer_at_least(buf->buffer.size()),
            boost::bind(
            &asio_message_socket::on_send,
            this->shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred,
            buf
            )
            );
#endif
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
    std::list<message_data_ptr> m_free_receive_buffers;
};


}
