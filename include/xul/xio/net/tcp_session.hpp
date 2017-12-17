#pragma once

#include <xul/net/tcp_socket.hpp>
#include <xul/xio/io_session_adapter.hpp>
#include <xul/io/codec/message_decoder.hpp>
#include <xul/util/time_counter.hpp>
#include <xul/macro/minmax.hpp>
#include <xul/lang/object_ptr.hpp>
#include <xul/util/log.hpp>
#include <boost/noncopyable.hpp>
#include <stdint.h>
#include <assert.h>

namespace xul {


class tcp_session
    : public io_session_adapter
    , public xul::tcp_socket_listener
{
public:
    explicit tcp_session(tcp_socket* sock)
        : m_socket(sock)
        , m_aborted(false)
    {
        XUL_LOGGER_INIT("tcp_session");
        XUL_DEBUG("new");
        m_receive_buffer_size = 2048;
        m_socket->set_listener(this);
        //m_socket->set_max_sending_queue_size(10);
        m_pause_receive = false;
    }
    virtual ~tcp_session()
    {
        XUL_DEBUG("delete");
        m_socket->reset_listener();
        if (do_get_message_decoder())
            do_get_message_decoder()->set_listener(NULL);
    }

    virtual void destroy()
    {
        this->set_listener(NULL);
        this->close();
    }
    void pause_receive()
    {
        m_pause_receive = true;
    }

    void resume_receive()
    {
        if (false == m_pause_receive)
            return;
        m_pause_receive = false;
        if (m_socket && is_open())
        {
            this->read();
        }
    }

    virtual bool get_local_address(socket_address& addr) const
    {
        return m_socket->get_local_address(addr);
    }
    virtual bool get_remote_address(socket_address& addr) const
    {
        return m_socket->get_remote_address(addr);
    }

    virtual bool write(const uint8_t* data, size_t size)
    {
        return m_socket->send(data, size);
    }
    virtual void read(size_t size)
    {
        assert(!m_socket->is_receiving());
        m_socket->receive(size);
    }
    virtual void read()
    {
        if (m_socket->is_receiving())
            return;
        m_socket->receive(m_receive_buffer_size);
    }
    virtual bool write_n(const uint8_t* data, size_t size)
    {
        return m_socket->send_n(data, size);
    }
    virtual void read_n(size_t size)
    {
        assert(!m_socket->is_receiving());
        m_socket->receive_n(size);
    }
    virtual int get_writing_queue_size() const
    {
        return m_socket->get_sending_queue_size();
    }
    virtual void set_max_writing_queue_size(int queue_size) const
    {
        m_socket->set_max_sending_queue_size(queue_size);
    }

    void set_receive_buffer_size(size_t recvBufSize)
    {
        XUL_DEBUG("set_receive_buffer_size " << recvBufSize);
        assert(recvBufSize >= 512);
        m_receive_buffer_size = recvBufSize;
        XUL_LIMIT_MIN(m_receive_buffer_size, 1);
    }

    tcp_socket* get_socket() { return m_socket.get(); }

    virtual void close()
    {
        abort();
        if (m_socket)
        {
            m_socket->close();
        }
        io_session_adapter::close();
    }

    virtual void abort()
    {
        m_aborted = true;
        do_get_message_decoder()->abort();
    }

    virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)
    {
        XUL_DEBUG("on_socket_receive " << size);
        //printf("on_socket_receive %d\n", size);
        assert(m_socket.get() == sender);
        boost::intrusive_ptr<message_decoder> decoder = do_get_message_decoder();
        XUL_DEBUG("on_socket_receive decoder " << decoder);
        if (decoder->get_remaining_size() > 0)
        {
            XUL_WARN("possible error " << decoder->get_remaining_size());
        }
        decoder->feed(data, size);
        XUL_DEBUG("on_socket_receive decoder ok " << decoder);
        if (m_aborted || decoder->is_aborted())
            return;
        if (m_pause_receive)
            return;
        //XUL_DEBUG("socket start receive " << m_receive_buffer_size);
        //m_socket->receive(m_receive_buffer_size);
        this->read();
    }

    virtual void on_socket_send_failed(tcp_socket* sender, int errcode)
    {
        on_session_error(errcode);
    }
    virtual void on_socket_receive_failed(tcp_socket* sender, int errcode)
    {
        on_session_error(errcode);
    }
    virtual void on_socket_send(tcp_socket* sender, size_t bytes)
    {
        do_get_listener()->on_session_send(this, bytes);
    }

    virtual void on_decoder_message(xul::message_decoder* decoder, xul::decoder_message_base* msg)
    {
        do_get_listener()->on_session_receive(this, msg);
    }
    virtual void on_decoder_error(xul::message_decoder* decoder, int errcode)
    {
        on_session_error(errcode);
    }

    virtual bool is_open() const
    {
        return m_socket->is_open() && m_socket->is_connected();
    }

    //virtual bool is_writable() const
    //{
    //    return false == m_socket->is_sending();
    //}
    virtual bool is_readable() const
    {
        return false == m_socket->is_receiving();
    }

    virtual void attach_listener()
    {
        io_session_adapter::attach_listener();
    }

    virtual void open_session()
    {
        m_aborted = false;
        m_pause_receive = false;
        io_session_adapter::open_session();
        this->read();
    }
protected:

    virtual void on_session_error(int errcode)
    {
        do_get_listener()->on_session_error(this, errcode);
    }

protected:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<tcp_socket> m_socket;

    size_t m_receive_buffer_size;

    xul::time_counter m_start_time;

    bool m_aborted;

    bool m_pause_receive;
};


}
