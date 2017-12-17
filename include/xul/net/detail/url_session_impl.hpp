#pragma once

#include <xul/net/url_session.hpp>
#include <xul/net/tcp_socket.hpp>
#include <xul/net/url_messages.hpp>
#include <xul/net/url_response.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/data/printables.hpp>
#include <xul/log/log.hpp>
#include <xul/data/buffer.hpp>
#include <xul/macro/foreach.hpp>
#include <set>


namespace xul {


/// handles a single url request from client
class url_session_impl : public object_impl<url_session>
{
public:
    explicit url_session_impl(tcp_socket* conn)
        : m_connection(conn)
        , m_keep_alive(false)
        , m_response(create_url_response())
    {
        XUL_LOGGER_INIT("url_session");
        XUL_DEBUG("new");
        this->reset();
    }
    ~url_session_impl()
    {
        XUL_DEBUG("delete");
        XUL_FOREACH_CONST(std::set<url_session_listener*>, m_listeners, iter)
        {
            url_session_listener* listener = *iter;
            listener->on_session_close(this);
        }
    }

    virtual tcp_socket* get_connection()
    {
        return m_connection.get();
    }
    virtual url_response* get_response()
    {
        return m_response.get();
    }
    virtual void reset()
    {
        m_finished = false;
        m_chunked_mode = false;
        m_header_finished = false;
        m_chunked_buffer.resize(0);
        m_response->clear();
    }
    virtual bool is_finished() const
    {
        return m_finished;
    }
    virtual bool is_keep_alive() const
    {
        return m_keep_alive;
    }
    virtual void set_keep_alive(bool keep_alive)
    {
        m_keep_alive = keep_alive;
    }

    virtual bool send_header()
    {
        //assert(!m_header_finished);
        if (m_chunked_mode)
        {
            url_messages::set_chunked(*m_response);
            assert(url_messages::get_content_length(*m_response) < 0);
        }
        std::string str = printables::to_string(*m_response);
        XUL_DEBUG("send_header " << str);
        if (false == do_send_data((const uint8_t*)str.data(), str.size()))
        {
            assert(false);
            return false;
        }
        m_header_finished = true;
        return true;
    }
    virtual bool send_data(const uint8_t* data, int size)
    {
        assert(m_header_finished);
        if (m_chunked_mode)
        {
            char len_tag[16] = { 0 };
            sprintf(len_tag, "%x\r\n", size);
            std::string len_tag_str = len_tag;
            xul::byte_buffer& buf = m_chunked_buffer;
            buf.reserve(size + len_tag_str.size() +  2);
            buf.resize(0);
            buf.assign((const uint8_t*)len_tag_str.data(), len_tag_str.size());
            buf.append(data, size);
            buf.append((const uint8_t*)"\r\n", 2);
            if (false == do_send_data(buf.data(), buf.size()))
            {
                return false;
            }
            return true;
        }
        return do_send_data(data, size);
    }
    virtual void close()
    {
        XUL_DEBUG("close");
        if (m_connection)
            m_connection->close();
    }
    virtual void finish()
    {
        if (!m_connection)
        {
            assert(false);
            return;
        }
        XUL_DEBUG("finish " << m_connection->get_sending_queue_size());
        //assert(m_connection->get_sending_queue_size() > 0);
        if (m_chunked_mode)
        {
            get_connection()->set_max_sending_queue_size(m_connection->get_sending_queue_size() + 3);
            const char* chunked_end_tag = "0\r\n\r\n";
            bool ret = do_send_data((const uint8_t*)chunked_end_tag, strlen(chunked_end_tag));
            assert(ret);
        }
        m_finished = true;
        if (m_connection->get_sending_queue_size() == 0 && !m_keep_alive)
        {
            XUL_DEBUG("finish: close connection");
            m_connection->close();
        }
    }
    virtual bool is_chunked() const
    {
        return m_chunked_mode;
    }
    virtual void set_chunked(bool enabled)
    {
        if (m_header_finished)
        {
            assert(false);
            return;
        }
        m_chunked_mode = enabled;
    }

    bool do_send_data(const uint8_t* data, int size)
    {
        return m_connection->send_n(data, size);
    }
    virtual void register_listener(url_session_listener* listener)
    {
        if (listener)
        {
            m_listeners.insert(listener);
        }
    }
    virtual void unregister_listener(url_session_listener* listener)
    {
        m_listeners.erase(listener);
    }

private:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<tcp_socket> m_connection;
    boost::intrusive_ptr<url_response> m_response;
    bool m_finished;
    bool m_header_finished;
    bool m_chunked_mode;
    bool m_keep_alive;
    xul::byte_buffer m_chunked_buffer;
    std::set<url_session_listener*> m_listeners;
};


}
