#pragma once

#include <xul/xio/net/url_handler_impl.hpp>
#include <xul/net/http/url_data_input_stream.hpp>
#include <xul/net/http/http_request.hpp>
#include <xul/util/log.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class uri;
class http_request;
class http_response;
class http_content_range;


class stream_url_handler
    : public url_handler_impl
    , public url_data_input_stream_listener
{
public:
    explicit stream_url_handler(boost::shared_ptr<url_data_input_stream> stream)
        : m_stream(stream)
    {
        XUL_DEBUG("stream_url_handler construct " << this);
        //m_reading = false;
        m_sent_header = false;
        m_total_read_size = 0;
        m_stream->set_listener(this);
    }
    virtual ~stream_url_handler()
    {
        if (m_stream)
        {
            // cancel pending data callback
            m_stream->reset_listener();
        }

        XUL_DEBUG("stream_url_handler destruct " << this);
    }

    virtual void handle_request(io_session* session, message_decoder* decoder, const http_request& req)
    {
        m_session = session;
        m_client_request = req;
        m_request_range.clear();
        m_client_request.get_range(m_request_range);
        int64_t start_pos = m_request_range.get_real_start();
        //m_stats.set_start_position(start_pos);
        //m_stats.start_time.sync();
        m_sent_header = false;
        set_content_length(0);
        m_stream->open(session, req);
    }
    virtual void handle_data_sent(io_session* session, int bytes)
    {
        //url_handler::on_response_send(session, bytes);
        if (false == session->is_open())
        {
            XUL_WARN("on_response_send closed " << this);
            return;
        }
        XUL_DEBUG("on_response_send " << xul::make_tuple(bytes, session->get_writing_queue_size()));
        //update_stats();
        //send_data();
        int queue_size = session->get_writing_queue_size();
        int free_size = 20 - queue_size;
        for (int i = 0; i < free_size; ++i)
        {
            if (false == read_data())
                break;
        }
    }

    virtual void on_stream_open(url_data_input_stream*, const std::string& content_type, int64_t total_size)
    {
        XUL_DEBUG("on_stream_open " << xul::make_tuple(content_type, total_size));
        //assert(m_acca);
        //assert(m_stream);
        http_response resp;
        init_response(resp, m_request_range, total_size);
        resp.set_header("Server", "nginx/1.0.4");
        resp.set_content_type(content_type);
        if (m_request_range.is_valid())
        {
        }
        send_response(get_session(), resp, get_content_length());
        m_sent_header = true;
        if (get_content_length() > 0)
        {
            read_data();
        }
        //m_stream->read();
    }
    virtual void on_stream_read(url_data_input_stream* sender, const uint8_t* data, size_t size)
    {
        if (!get_session())
        {
            //assert(false);
            return;
        }
        //if (m_buffers.size() > 10)
        //    return;
        if (get_session()->get_writing_queue_size() > 40)
        {
            assert(false);
            return;
        }
        //update_stats();
        m_total_read_size += size;
        //assert(pos >= m_request_range.get_real_start());
        XUL_DEBUG("on_data_read " << xul::make_tuple(size, get_session()->get_writing_queue_size()));
        //if (false == m_reading)
        {
        //    assert(false);
        //    return;
        }
        //m_reading = false;
        if (0 == size)
            return;
        //if (errcode != 0)
        {
            //get_session()->close();
            //    retry_read();
            //    return;
        }
        get_session()->write_n(data, size);
#if 0
        buffer_ptr buf(new send_buffer_info);
        buf->data.assign(data, data + size);
        buf->position = 0;
        m_buffers.push_back(buf);
        send_data();
#endif
    }
    virtual void on_stream_read_failed( url_data_input_stream*, int errcode )
    {
        if (get_session())
        {
            get_session()->close();
        }
    }
    virtual void on_stream_open_failed( url_data_input_stream*, int errcode )
    {
        if (get_session())
        {
            if (errcode > 100 && errcode < 600)
            {
                send_error_response(get_session(), errcode);
                return;
            }
            get_session()->close();
        }
    }
    virtual void on_stream_finished(url_data_input_stream*)
    {
        if (get_to_send_size() < 0)
        {
            set_real_content_length(m_total_read_size);
            if (get_session() && get_session()->get_writing_queue_size() == 0)
                check_close_finished(get_session());
        }
    }

private:
    bool read_data()
    {
        if (!m_stream || !get_session())
            return false;
        XUL_DEBUG("read_data " << xul::make_tuple(m_stream, get_session()->get_writing_queue_size()));
        //if (m_reading)
        //    return;
        //if (m_buffers.size() > 2)
        //    return;
        if (false == get_session()->is_open())
            return false;
        if (get_session()->get_writing_queue_size() > 20)
            return false;
        //m_reading = true;
        //m_stream->async_read_n(readSize, this);
        return m_stream->read();
    }
#if 0
    void send_data()
    {
        if (!get_session() || false == get_session()->is_writable())
        {
            return;
        }
        if (m_buffers.empty())
        {
            read_data();
            return;
        }
        buffer_ptr buf = m_buffers.front();
        assert(get_session()->get_writing_queue_size() < 90);
        get_session()->write_n(&(buf->data)[0], buf->data.size());
        XUL_DEBUG("media send " << buf->data.size() << " bytes at position " << buf->position);
        //assert(buf->position >= m_request_range.get_real_start());
        m_buffers.pop_front();
        //read_data();
    }
#endif

private:
    class send_buffer_info
    {
    public:
        std::vector<uint8_t> data;
        int64_t position;
    };
    boost::shared_ptr<url_data_input_stream> m_stream;
    //bool m_reading;
    //typedef boost::shared_ptr<send_buffer_info> buffer_ptr;
    //std::list<buffer_ptr> m_buffers;

    http_request m_client_request;
    http_range m_request_range;
    bool m_sent_header;
    int64_t m_total_read_size;

    inet_socket_address m_local_address;
    inet_socket_address m_remote_address;
};


}
