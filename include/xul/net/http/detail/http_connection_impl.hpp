#pragma once

#include <xul/net/http/detail/inner_http_connection.hpp>
#include <xul/net/http/detail/http_connection_request_info.hpp>
#include <xul/net/http/detail/http_filter.hpp>
#include <xul/net/http/detail/http_connection_options_impl.hpp>
#include <xul/net/http/detail/dummy_http_connection_listener.hpp>
#include <xul/net/io_services.hpp>
#include <xul/net/url_request.hpp>
#include <xul/net/http/http_range.hpp>
#include <xul/net/http/http_method.hpp>
#include <xul/net/url_messages.hpp>
#include <xul/net/uri.hpp>
#include <xul/net/name_resolver.hpp>
#include <xul/data/printables.hpp>
#include <xul/macro/iterate.hpp>
#include <xul/log/log.hpp>
#include <boost/asio/error.hpp>
#include <boost/bind.hpp>


namespace xul { namespace detail {


class http_connection_impl 
    : public object_impl<inner_http_connection>
    , public name_resolver_listener
    , public tcp_socket_listener
{
public:
    class connection_state
    {
    public:
        bool pause_receive;

        connection_state()
        {
            reset();
        }
        void reset()
        {
            pause_receive = false;
        }
    };

    explicit http_connection_impl(io_service* ios) 
        : m_io_service(ios)
        , m_options(new http_connection_options_impl)
        , m_port(0)
    {
        XUL_LOGGER_INIT("http_connection");
        XUL_DEBUG("new " << m_options->receive_buffer_size);
        set_listener(NULL);
        init_client();
    }
    virtual ~http_connection_impl()
    {
        XUL_DEBUG("delete");
        this->destroy();
    }
    virtual bool request(const char* method, const char* urlstr, const string_table* headers)
    {
        http_connection_request_info_ptr reqInfo = prepare_request(urlstr, method, headers);
        return do_request(reqInfo.get());
    }
    virtual bool download(const char* urlstr, int64_t pos, int64_t len, const string_table* headers)
    {
        if (0 == len)
        {
            assert(!"http_connection.download: len is zero");
            post_error(http_error_codes::invalid_range_size);
            return false;
        }
        http_connection_request_info_ptr reqInfo = prepare_request(urlstr, XUL_HTTP_GET, headers);
        if (reqInfo)
            xul::url_messages::set_range(*reqInfo->request, pos, len);
        return do_request(reqInfo.get());
    }

    virtual bool send_data(const uint8_t* data, int size)
    {
        if (!is_started())
            return false;
        if (!m_request || !m_request->header_send)
        {
            assert(false);
            return false;
        }
        return m_socket->send_n(data, size);
    }

    virtual bool is_paused() const
    {
        return m_state.pause_receive;
    }
    virtual void pause()
    {
        m_state.pause_receive = true;
    }
    virtual void resume()
    {
        m_state.pause_receive = false;
        check_receive();
    }

    virtual bool is_started() const
    {
        return m_request.get() != NULL;
    }
    virtual bool is_requesting() const
    {
        return m_request.get() != NULL;
    }
    virtual bool is_open() const
    {
        return m_socket->is_connected();
    }
    virtual bool is_connected() const
    {
        return m_socket->is_connected();
    }

    virtual void close()
    {
        XUL_DEBUG("close " << is_started() << xul::make_tuple(m_request, m_socket, m_socket->is_connected()));
        m_request = NULL;
        m_state.reset();
        m_socket->destroy();
        if (m_name_resolver)
        {
            m_name_resolver->destroy();
            m_name_resolver = NULL;
        }
        init_client();
    }
    virtual void destroy()
    {
        set_listener(NULL);
        close();
    }

    virtual void set_listener(http_connection_listener* listener)
    {
        m_handler = NULL;
        m_listener = listener ? listener : dummy_http_connection_listener::instance();
    }
    virtual void set_handler(xul::http_connection_handler* handler)
    {
        if (handler)
        {
            m_handler = handler;
            m_listener = handler;
            return;
        }
        m_listener = xul::dummy_http_connection_listener::instance();
        m_handler = NULL;
    }
    const http_connection_listener* get_listener() const { return m_listener; }

    void write_request(http_connection_request_info* reqInfo)
    {
        assert(reqInfo->url->get_host() == m_host && reqInfo->url->get_port() == m_port);
        assert(!reqInfo->header_send);
        std::string msg = printables::to_string(*reqInfo->request);
        XUL_DEBUG("write_request \n" << msg);
        reqInfo->header_send = true;
        m_socket->send_n(reinterpret_cast<const uint8_t*>(msg.data()), msg.size());
        m_listener->on_http_put_content(this);
    }
    void handle_error(int errcode)
    {
        if (!m_request)
        {
            XUL_ERROR("handle_error with idle connection " << errcode);
            close();
            return;
        }
        http_connection_request_info_ptr reqInfo = m_request;
        XUL_ERROR("handle_error " << errcode);
        if (reqInfo->response_header_received && (boost::asio::error::eof == errcode))
        {
            reqInfo->filters->finish(reqInfo->request.get(), reqInfo->response_extractor->get_response(), NULL, 0);
            return;
        }
        on_error(errcode);
    }

    virtual const inet_socket_address& get_local_address() const
    {
        return m_local_address;
    }
    virtual const inet_socket_address& get_remote_address() const
    {
        return m_remote_address;
    }
    virtual void set_receive_buffer_size(int bufsize)
    {
        XUL_DEBUG("set_receive_buffer_size " << bufsize);
        m_options->set_receive_buffer_size(bufsize);
    }

    void post_error(int errcode)
    {
        xul::io_services::post_runnable(m_io_service.get(), boost::bind(&http_connection_impl::on_error, this, errcode));
    }

    virtual const http_connection_options* get_options() const { return m_options.get(); }
    virtual http_connection_options* ref_options() { return m_options.get(); }

    virtual void on_socket_connect(tcp_socket* sender)
    {
        m_socket->get_local_address(m_local_address);
        m_socket->get_remote_address(m_remote_address);
        m_socket->receive(m_options->receive_buffer_size);
        if (!m_request)
        {
            assert(false);
            close();
            return;
        }
        write_request(m_request.get());
    }
    virtual void on_socket_connect_failed(tcp_socket* sender, int errcode)
    {
        handle_error(errcode);
    }
    virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)
    {
        XUL_DEBUG("on_socket_receive " << size << " " << this);
        if (!m_request)
        {
            XUL_ERROR("on_socket_receive no request, abort");
            close();
            return;
        }
        http_connection_request_info_ptr reqInfo = m_request;
        
        if (!reqInfo->process(data, size))
        {
            XUL_ERROR("on_socket_receive process abort");
            return;
        }
        if (!sender->is_open())
            return;
        check_receive();
    }
    virtual void on_socket_receive_failed(tcp_socket* sender, int errcode)
    {
        handle_error(errcode);
    }
    virtual void on_socket_send(tcp_socket* sender, size_t bytes)
    {
    }

    bool on_complete(url_request* req, url_response* resp, int64_t total_size, const uint8_t* data, int size)
    {
        XUL_DEBUG("on_complete " << m_request << " " << xul::make_tuple(total_size, size));
        if (!m_request)
        {
            //assert(false);
            abort_handling();
            return false;
        }
        http_connection_request_info_ptr reqInfo = m_request;
        m_request = NULL;
        m_listener->on_http_complete(this, total_size);
        assert(0 == size);
        return true;
    }
    bool on_header(url_request* req, url_response* resp)
    {
        XUL_DEBUG("on_header " << m_request << " " << resp->get_status_code());
        if (!m_request)
        {
            //assert(false);
            abort_handling();
            return false;
        }
        if (!m_listener->on_http_response(this, resp, m_request->url->get_original_string()))
            return false;
        return true;
    }
    bool on_content(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        XUL_DEBUG("on_content " << m_request << " " << size);
        if (!m_request)
        {
            //assert(false);
            abort_handling();
            return false;
        }
        if (!m_listener->on_http_data(this, data, size))
            return false;
        return true;
    }
    bool on_error(int errcode)
    {
        XUL_ERROR("on_error " << m_request << " " << errcode);
        abort_handling();
        m_listener->on_http_error(this, errcode);
        return false;
    }
    virtual logger* get_logger() const { return m_xul_logger.get(); }

protected:
    void check_receive()
    {
        if (!m_state.pause_receive && m_socket && m_socket->is_connected() && !m_socket->is_receiving())
        {
            m_socket->receive(m_options->receive_buffer_size);
        }
    }

    http_connection_request_info_ptr prepare_request(const char* urlstr, const char* method, const string_table* headers)
    {
        boost::intrusive_ptr<uri> url = create_uri();
        if (!url->parse(urlstr) || !method || strcmp(method, "") == 0)
        {
            XUL_REL_ERROR("invalid request " << method << " " << headers << " " << urlstr);
            post_error(http_error_codes::invalid_url);
            assert(false);
            return http_connection_request_info_ptr();
        }
        http_connection_request_info_ptr reqInfo(new http_connection_request_info(this, url.get()));
        reqInfo->prepare_request(headers);
        reqInfo->request->set_method(method);
        return reqInfo;
    }

    bool need_reconnect(http_connection_request_info* reqInfo) const
    {
        if (!m_options->is_keep_alive())
            return true;
        if (!is_connected())
            return true;
        if (!is_same_host(reqInfo))
            return true;
        return is_requesting();
    }
    bool is_same_host(http_connection_request_info* reqInfo) const
    {
        if (reqInfo->url->get_host() == m_host && reqInfo->url->get_port() == m_port)
            return true;
        return false;
    }
    bool redirect(http_connection_request_info* reqInfo)
    {
        XUL_DEBUG("redirect " << m_request << " " << reqInfo->url->get_original_string());
        close();
        do_request(reqInfo);
        return true;
    }
    virtual void abort_handling()
    {
        XUL_WARN("abort_handling " << m_request);
        close();
    }
    bool do_request(http_connection_request_info* reqInfo)
    {
        if (!reqInfo)
        {
            assert(false);
            return false;
        }
        XUL_DEBUG("do_request " << xul::make_tuple(m_request, m_socket->is_connected()) 
            << " " << m_host << ":" << m_port 
            << " " << reqInfo->url->get_original_string());
        XUL_DEBUG("async_download " << reqInfo->request->get_header("Range", "") << " " << reqInfo->url->get_original_string());
        if ( need_reconnect(reqInfo) )
        {
            connect_host(reqInfo);
        }
        else
        {
            XUL_DEBUG("use existing connection");
            m_request = reqInfo;
            write_request(reqInfo);
            check_receive();
        }
        return true;
    }

    void init_client()
    {
        m_host_address.clear();
        m_host.clear();
        m_port = 0;
        m_state.reset();
        {
            if (m_socket)
                m_socket->destroy();
            m_socket = m_io_service->create_tcp_socket();
            m_socket->set_listener(this);
        }
    }
    void connect_host(http_connection_request_info* reqInfo)
    {
        assert(reqInfo);
        assert(!std::string(reqInfo->url->get_host()).empty());
        assert(reqInfo->url->get_port() > 0);
        close();
        m_request = reqInfo;
        m_host = m_request->url->get_host();
        m_port = m_request->url->get_port();
        XUL_DEBUG("connect_host " << m_host);
        m_host_address.set_ip(m_host.c_str());
        m_host_address.set_port(m_port);
        assert(m_host_address.get_port() > 0 && m_host_address.get_port() < 65535);
        if (m_host_address.get_inet_address().is_none())
        {
            if (m_name_resolver)
                m_name_resolver->destroy();
            m_name_resolver = m_io_service->create_name_resolver();
            m_name_resolver->set_listener(this);
            m_name_resolver->async_resolve(m_host.c_str());
            return;
        }
        connecto_host_address();
    }
    void connecto_host_address()
    {
        XUL_DEBUG("connecto_host_address " << m_host_address);
        assert(m_host_address.get_inet_address().is_valid());
        m_socket->connect(m_host_address);
    }
    virtual void on_resolver_address(name_resolver* sender, const std::string& name, int errcode, const std::vector<inet4_address>& addrs)
    {
        if (errcode != 0 || addrs.empty())
        {
            XUL_REL_ERROR("failed to resolve host name " << name << " " << xul::make_tuple(errcode, addrs.size()));
            handle_error(errcode);
            return;
        }
        XUL_REL_INFO("on_resolver_address " << name << " " << xul::make_tuple(errcode, addrs.size()) << " " << (addrs.empty() ? std::string() : addrs[0].str()));
        m_host_address.set_raw_ip(addrs[0].get_raw_address());
        connecto_host_address();
    }

private:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<io_service> m_io_service;
    boost::intrusive_ptr<tcp_socket> m_socket;
    boost::intrusive_ptr<name_resolver> m_name_resolver;
    inet_socket_address m_host_address;
    http_connection_listener* m_listener;
    boost::intrusive_ptr<xul::http_connection_handler> m_handler;
    connection_state m_state;
    boost::intrusive_ptr<http_connection_options_impl> m_options;
    std::string m_host;
    int m_port;
    inet_socket_address m_remote_address;
    inet_socket_address m_local_address;
    http_connection_request_info_ptr m_request;
};


} }
