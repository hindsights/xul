#pragma once

#include <xul/net/http/http_connection.hpp>
#include <xul/net/url_request.hpp>
#include <xul/net/uri.hpp>
#include <xul/util/timer_holder.hpp>
#include <xul/util/runnable_callback.hpp>
#include <xul/net/http/http_content_type.hpp>
#include <xul/net/http/http_method.hpp>
#include <xul/net/url_messages.hpp>
#include <xul/net/url_response.hpp>
#include <xul/data/printables.hpp>
#include <xul/data/buffer.hpp>
#include <xul/util/log.hpp>
#include <xul/lang/object_ptr.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/error.hpp>
#include <string>


namespace xul {


class http_client : public object_impl<object>
{
public:
    typedef boost::function<void (http_client*, uri*, int, url_response*, uint8_t*, int)> download_data_callback_type;
    typedef boost::function<void (http_client*, uri*, int, url_response*, int64_t)> query_head_callback_type;

    enum error_code
    {
        error_general = -1,
        error_timeout = -2,
    };

    class http_handler : private boost::noncopyable, public timer_listener, public http_connection_listener_adapter
    {
    public:
        explicit http_handler(http_client& owner) : m_owner(owner)
        {
            m_xul_logger = owner.m_xul_logger;
            m_content_length = 0;
        }
        virtual ~http_handler()
        {
        }
        void set_extra_header(const string_table* extra_headers)
        {
            m_extra_headers = extra_headers;
        }
        void request(uri* u)
        {
            m_url = u;
            m_owner.m_connection->request(m_method.c_str(), m_url->get_original_string(), m_extra_headers.get());
        }
        virtual void on_timer_elapsed(timer* sender)
        {
            handle_error(error_timeout);
        }
        virtual void do_handle_response() = 0;
        virtual bool on_http_response(http_connection* sender, url_response* resp, const char* real_url)
        {
            m_response = resp;
            m_content_length = url_messages::get_content_length(*resp);
            XUL_DEBUG("on_http_response content_length=" << m_content_length << " " << *resp);
            do_handle_response();
            return true;
        }
        virtual bool on_http_data(http_connection* sender, const uint8_t* data, int size)
        {
            return false;
        }
        virtual void on_http_complete(http_connection* sender, int64_t size)
        {
        }
        virtual void on_http_error(http_connection* sender, int errcode)
        {
            handle_error(errcode);
        }

        virtual void handle_error(int errcode) = 0;

    protected:
        XUL_LOGGER_DEFINE();
        http_client& m_owner;
        boost::intrusive_ptr<uri> m_url;
        boost::intrusive_ptr<url_response> m_response;
        int64_t m_content_length;
        boost::intrusive_ptr<const string_table> m_extra_headers;
        std::string m_method;
        //timer_ptr m_timeout_timer;
    };

    class http_download_data_handler : public http_handler
    {
    public:
        explicit http_download_data_handler(http_client& owner, int maxSize, download_data_callback_type callback)
            : http_handler(owner), m_callback(callback)
        {
            m_method = http_method::get();
            m_max_size = maxSize;
            m_recv_size = 0;
        }
        virtual void do_handle_response()
        {
            m_data.resize(0);
            if (m_content_length < 0)
            {
                XUL_WARN("http_client do_handle_response no content-length from " << m_url->get_original_string());
            }
            if (m_content_length > m_max_size)
            {
                XUL_WARN("content_length is too large " << xul::make_tuple(m_content_length, m_max_size));
                handle_error(-1);
                return;
            }
            if (m_content_length > 0)
                m_data.reserve(m_content_length);
        }
        virtual bool on_http_data(http_connection* sender, const uint8_t* data, int size)
        {
            m_data.append(data, size);
            if (m_data.size() > m_max_size)
            {
                XUL_REL_ERROR("recv_size is too large " << xul::make_tuple(m_data.size(), m_max_size));
                handle_error(-1);
                return false;
            }
            return true;
        }
        virtual void on_http_complete(http_connection* sender, int64_t size)
        {
            invoke_callback(0);
        }
        virtual void handle_error(int errcode)
        {
            m_owner.close_connection();
            if (boost::asio::error::eof == errcode && m_content_length < 0)
            {
                invoke_callback(0);
                return;
            }
            invoke_callback(errcode);
        }

    protected:
        void invoke_callback(int errcode)
        {
            m_owner.handle_complete();
            m_callback(&m_owner, m_url.get(), errcode, m_response.get(), m_data.data(), m_data.size());
        }

    private:
        download_data_callback_type m_callback;
        byte_buffer m_data;
        int m_max_size;
        int m_recv_size;
    };

    class http_query_head_handler : public http_handler
    {
    public:
        explicit http_query_head_handler(http_client& owner, query_head_callback_type callback)
            : http_handler(owner), m_callback(callback)
        {
            m_method = http_method::head();
        }
        virtual void do_handle_response()
        {
            m_owner.handle_complete();
            m_callback(&m_owner, m_url.get(), 0, m_response.get(), m_content_length);
        }
        virtual void handle_error(int errcode)
        {
            m_owner.handle_complete();
            m_callback(&m_owner, m_url.get(), errcode, m_response.get(), 0);
        }

    private:
        query_head_callback_type m_callback;
    };

    class http_post_data_handler : public http_download_data_handler
    {
    public:
        explicit http_post_data_handler(http_client& owner, int maxSize, download_data_callback_type callback
            , const std::string& bodyData, const std::string& contentType = std::string())
            : http_download_data_handler(owner, maxSize, callback)
            , m_post_body_data(bodyData)
            , m_content_type(contentType)
        {
            m_method = http_method::post();
            if (m_content_type.empty())
            {
                m_content_type = http_content_type::octet_stream();
            }
        }
        virtual void on_http_put_content(http_connection* sender)
        {
            sender->send_data(reinterpret_cast<const uint8_t*>(m_post_body_data.data()), m_post_body_data.size());
        }

    private:
        std::string m_post_body_data;
        std::string m_content_type;
    };

    explicit http_client(io_service* ios, bool http11 = false, bool gzip_enabled = false)
        : m_ios(ios)
        , m_connection(create_http_connection(ios))
        , m_keepalive(false)
    {
        XUL_LOGGER_INIT("http_client");
        m_receive_buffer_size = 32 * 1024;
        set_keepalive(false);
        m_connection->ref_options()->set("gzip", gzip_enabled ? "on" : "off");
        m_connection->ref_options()->set_protocol_version(http11 ? "1.1" : "1.0");
        m_connection->set_receive_buffer_size(m_receive_buffer_size);
        m_connection->set_decoder_buffer_size(m_receive_buffer_size + 1000);
    }
    virtual ~http_client()
    {
        if (m_connection)
        {
            m_connection->set_listener(NULL);
        }
        this->clear();
    }
    
    void set_keepalive(bool keepalive)
    {
        set_keep_alive(keepalive);
    }
    void set_keep_alive(bool keepalive)
    {
        m_keepalive = keepalive;
        if (m_connection)
            m_connection->ref_options()->set_keep_alive(m_keepalive);
    }

    void clear()
    {
        m_http_handler.reset();
        this->close();
    }

    bool is_started() const
    {
        return m_connection && m_connection->is_started();
    }

    void set_receive_buffer_size(size_t bufsize)
    {
        if (bufsize < 1024 || bufsize > 2 * 1024 * 1024)
            return;
        m_receive_buffer_size = bufsize;
        if (m_connection)
            m_connection->set_receive_buffer_size(m_receive_buffer_size);
    }

    void handle_complete()
    {
        XUL_DEBUG("handle_complete");
        if (!m_keepalive)
        {
            close_connection();
        }
    }
    void close_connection()
    {
        if (m_connection)
        {
            m_connection->set_listener(NULL);
            m_connection->close();
        }
    }
    void close()
    {
        XUL_DEBUG("close");
        close_connection();
        //m_http_handler.reset();
    }
    //void async_download_data(const char* host, int port, const url_request* req, int maxSize, download_data_callback_type callback)
    //{
    //    close();
    //    m_http_handler.reset(new http_download_data_handler(*this, maxSize, callback));
    //    async_execute(host, port, req);
    //}
    
    void async_download_data(const std::string& urlstr, size_t maxSize, download_data_callback_type callback,
                             const string_table* extra_header = NULL)
    {
        download_data(urlstr, maxSize, callback, extra_header);
    }
    void download_data(const std::string& urlstr, size_t maxSize, download_data_callback_type callback,
                             const string_table* extra_header = NULL)
    {
        boost::intrusive_ptr<uri> u = create_uri();
        if (false == u->parse(urlstr.c_str()))
        {
            uint8_t* buf = NULL;
            boost::intrusive_ptr<runnable> r = make_runnable(boost::bind(callback, this, u.get(), -1, (xul::url_response*)NULL, buf, 0));
            m_ios->post(r.get());
            return;
        }
        async_download_uri_data(u.get(), maxSize, callback, extra_header);
    }
    void async_download_uri_data(uri* u, size_t maxSize, download_data_callback_type callback,
        const string_table* extra_header = NULL)
    {
        XUL_DEBUG("async_download_uri_data " << u->get_original_string());
        //close();
        m_http_handler.reset(new http_download_data_handler(*this, maxSize, callback));
        if (extra_header)
        {
            m_http_handler->set_extra_header(extra_header);
        }
        async_download(u);
    }
    void async_query_head(uri* u, query_head_callback_type callback)
    {
        //close();
        m_http_handler.reset(new http_query_head_handler(*this, callback));
        async_download(u);
    }
    void async_post_data(uri* u, size_t maxSize, download_data_callback_type callback,
                         const std::string& body, const std::string& contentType = std::string(),
                         const string_table* extra_header = NULL)
    {
        post_data(u->get_original_string(), maxSize, callback, body, contentType, extra_header);
    }
    void async_post_data(const std::string& urlstr, size_t maxSize, download_data_callback_type callback,
                         const std::string& body, const std::string& contentType = std::string(),
                         const string_table* extra_header = NULL)
    {
        post_data(urlstr, maxSize, callback, body, contentType, extra_header);
    }
    void post_data(const std::string& urlstr, size_t maxSize, download_data_callback_type callback,
        const std::string& body, const std::string& contentType = std::string(),
        const string_table* extra_header = NULL)
    {
        boost::intrusive_ptr<uri> u = create_uri();
        u->parse(urlstr.c_str());
        //close();
        m_http_handler.reset(new http_post_data_handler(*this, maxSize, callback, body, contentType));
        boost::intrusive_ptr<string_table> headers;
        if (extra_header)
            headers = extra_header->clone();
        else
            headers = create_associative_istring_array();
        headers->set("Content-Length", numerics::format<int>(body.size()).c_str());
        m_http_handler->set_extra_header(headers.get());
        async_download(u.get());
    }
    const http_connection* get_connection() const { return m_connection.get(); }

protected:
    void async_download(uri* u)
    {
        XUL_DEBUG("async_download " << u->get_original_string());
        if (!m_http_handler)
        {
            assert(false);
            return;
        }
        m_connection->set_listener(m_http_handler.get());
        m_http_handler->request(u);
    }
    //void async_execute(const char* host, int port, const url_request* req)
    //{
    //    XUL_DEBUG("async_execute " << xul::make_tuple(host, port, req->get_url()));
    //    if (!m_http_handler)
    //    {
    //        assert(false);
    //        return;
    //    }
    //    m_connection->set_listener(m_http_handler.get());
    //    m_connection->execute(host, port, req);
    //}

private:
    XUL_LOGGER_DEFINE();

    boost::intrusive_ptr<io_service> m_ios;
    size_t m_receive_buffer_size;
    boost::intrusive_ptr<http_connection> m_connection;
    boost::shared_ptr<http_handler> m_http_handler;
    bool m_keepalive;
};


}
