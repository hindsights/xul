#pragma once

#include <xul/net/http/http_server.hpp>
#include <xul/net/http/http_server_router.hpp>
#include <xul/net/url_handler_adapter.hpp>
#include <xul/net/detail/url_session_impl.hpp>
#include <xul/net/url_sessions.hpp>
#include <xul/net/tcp_acceptor.hpp>
#include <xul/net/io_service.hpp>
#include <xul/util/timer_holder.hpp>
#include <xul/data/numerics.hpp>
#include <xul/xio/net/http_request_decoder.hpp>
//#include <xul/xio/net/http_request_handler.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>


namespace xul {


class http_server_options_impl
{
public:
    bool enable_reuse_address;
    int max_keep_alive_requests;
    int keep_alive_timeout;
    int receive_buffer_size;
    int max_header_size;
    int max_body_size;

    http_server_options_impl()
    {
        enable_reuse_address = false;
        max_keep_alive_requests = 100;
        keep_alive_timeout = 15;
#ifdef _DEBUG
        receive_buffer_size = 4*1024;
#else
        receive_buffer_size = 16*1024;
#endif
        max_header_size = 16 * 1024;
        max_body_size = 600*1024;
    }
};

class http_server_impl
    : public object_impl<http_server>
    , public tcp_acceptor_listener
{
public:
    typedef boost::function<url_handler* (url_session* session, const url_request* req)> url_handler_creator_type;

    class http_empty_router : public object_impl<http_server_router>
    {
    public:
        http_empty_router()
        {
        }
        virtual url_handler* create_url_handler(http_server* server, url_session* session, const url_request* req)
        {
            XUL_APP_REL_WARN("http_empty_router.create_url_handler " << req->get_url() << " " << session);
            return NULL;
        }
    };

    class http_server_client : private boost::noncopyable, public tcp_socket_listener
    {
    public:
        explicit http_server_client(http_server_impl& owner, tcp_socket* sock, const inet_socket_address& addr)
            : m_owner(owner)
            , m_socket(sock)
            , m_client_address(addr)
            , m_session(new url_session_impl(sock))
        {
            XUL_LOGGER_INIT("http_server_client");
            XUL_DEBUG("new");
            m_socket->set_listener(this);
            m_receiving_header = true;
            m_handled_requests = 0;
            m_content_length = 0;
            m_received_body_size = 0;
        }
        virtual ~http_server_client()
        {
            XUL_DEBUG("delete");
            m_socket->set_listener(NULL);
            close();
        }
        void start()
        {
            m_receiving_header = true;
            receive();
        }
        void close()
        {
            m_socket->close();
        }
        virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)
        {
            XUL_DEBUG("on_socket_receive " << xul::make_tuple(m_header_buffer.size(), size));
            if (m_receiving_header)
            {
                handle_header_data(data, size);
            }
            else
            {
                handle_body_data(data, size);
            }
        }
        virtual void on_socket_receive_failed(tcp_socket* sender, int errcode)
        {
            XUL_DEBUG("on_socket_receive_failed " << errcode);
            reset_state();
            if (m_handler)
            {
                m_handler->handle_error(m_session.get(), errcode);
                m_session->close();
            }
            m_owner.on_client_error(this, errcode);
        }
        virtual void on_socket_send(tcp_socket* sender, size_t bytes)
        {
            XUL_DEBUG("on_socket_send " << make_tuple(bytes, sender->get_sending_queue_size()));
            if (!m_handler)
                return;
            m_handler->handle_data_sent(m_session.get(), bytes);
            if (!m_session->get_connection())
                return;
            if (m_session->is_finished() && m_session->get_connection()->get_sending_queue_size() == 0)
            {
                XUL_DEBUG("request finshed " << m_current_request->get_url() << " " << xul::make_tuple(m_handled_requests, m_owner.get_options()->max_keep_alive_requests));
                if (!m_session->is_keep_alive() || m_handled_requests >= m_owner.get_options()->max_keep_alive_requests)
                {
                    m_session->get_connection()->close();
                }
                else
                {
                    reset_state();
                }
                m_handler->handle_request_finished(m_session.get());
                m_handler = NULL;
                return;
            }
        }
        virtual void on_socket_send_failed(tcp_socket* sender, int errcode)
        {
            XUL_DEBUG("on_socket_send_failed " << errcode);
            reset_state();
            if (m_handler)
            {
                m_handler->handle_error(m_session.get(), errcode);
                m_session->close();
            }
            m_owner.on_client_error(this, errcode);
        }

    private:
        void handle_error(int errcode)
        {
            reset_state();
            m_owner.on_client_error(this, -1);
        }
        void handle_header_data(const uint8_t* data, size_t size)
        {
            const char* bufstart = NULL;
            size_t bufsize = 0;
            if (m_header_buffer.empty())
            {
                parse_header_data(data, size);
            }
            else
            {
                m_header_buffer.append(data, size);
                parse_header_data(m_header_buffer.data(), m_header_buffer.size());
            }
        }
        void parse_header_data(const uint8_t* data, size_t bufsize)
        {
            const char* bufstart = (const char*)data;
            const char* pos = http_message::find_header_end(bufstart, bufsize);
            if (NULL == pos)
            {
                if (bufsize > m_owner.get_options()->max_header_size)
                {
                    XUL_ERROR("do not find http header ending tag " << xul::make_tuple(bufsize, m_owner.get_options()->max_header_size));
                    handle_error(-1);
                    return;
                }
                XUL_INFO("receive more header data " << bufsize);
                if (m_header_buffer.empty())
                    m_header_buffer.assign(data, bufsize);
                receive();
                return;
            }
            int headerSize = pos - bufstart + 4;
            XUL_DEBUG("find http request header " << xul::make_tuple(m_header_buffer.size(), bufsize, headerSize));
            boost::intrusive_ptr<url_request> request = create_url_request();
            http_request_parser parser(request.get());
            if (false == parser.parse(bufstart, headerSize))
            {
                XUL_ERROR("failed to parse http header " << xul::make_tuple(m_header_buffer.size(), bufsize, headerSize));
                handle_error(-1);
                return;
            }
            m_receiving_header = false;
            m_content_length = url_messages::get_content_length(*request);
            // if (m_content_length > m_owner.get_options()->max_body_size)
            // {
            //     XUL_ERROR("content length is too large " << xul::make_tuple(m_content_length, m_owner.get_options()->max_body_size));
            //     handle_error(-1);
            //     return;
            // }
            m_current_request = request;
            handle_request();
            assert(bufsize >= headerSize);
            if (bufsize > headerSize)
            {
                if (m_content_length == 0)
                {
                    // this request is finished, process next one
                    reset_state();
                    XUL_INFO("handle_body_data: finished requetst " << m_current_request << " " << m_handler << " " << m_content_length << " " << m_received_body_size);
                    handle_header_data(data + headerSize, bufsize - headerSize);
                }
                else
                {
                    // process request body
                    handle_body_data(data + headerSize, bufsize - headerSize);
                }
            }
            else
            {
                if (m_socket->is_open())
                    receive();
            }
        }
        void handle_request()
        {
            XUL_DEBUG("handle_request \n" << *m_current_request);
            ++m_handled_requests;
            m_session->reset();
            m_handler = m_owner.get_router()->create_url_handler(&m_owner, m_session.get(), m_current_request.get());
            if (!m_handler)
                m_handler = new not_found_handler;
            m_handler->handle_request(m_session.get(), m_current_request.get());
        }
        void handle_body_data(const uint8_t* data, size_t size)
        {
            XUL_DEBUG("handle_body_data " << size);
            assert(size > 0);
            //XUL_WARN("reject any further body data");
            if (!m_current_request || !m_handler)
            {
                XUL_WARN("handle_body_data: no current request no handler, reject any further body data" << m_current_request << " " << m_handler);
                m_socket->close();
                handle_error(-1);
                return;
            }
            XUL_INFO("handle_body_data: process body data" << m_current_request << " " << m_handler << " " << m_content_length << " " << m_received_body_size);
            int64_t remainingSize = m_content_length - m_received_body_size;
            int readSize = static_cast<int>(numerics::get_min<int64_t>(remainingSize, size));
            assert(readSize > 0);
            assert(readSize <= size);
            m_received_body_size += readSize;
            m_handler->handle_body_data(m_session.get(), data, readSize);
            // m_current_request->ref_body().append(reinterpret_cast<const char*>(data), readSize);
            if (size > readSize)
            {
                reset_state();
                XUL_INFO("handle_body_data: process body data" << m_current_request << " " << m_handler << " " << m_content_length << " " << m_received_body_size);
                handle_header_data(data + readSize, size - readSize);
            }
            else
            {
                if (readSize == remainingSize)
                {
                    // already read the remaining, then this request is over, wait for the next one
                    reset_state();
                }
                if (m_socket->is_open())
                    receive();
            }
        }
        void receive()
        {
            m_socket->receive(m_owner.get_options()->receive_buffer_size);
        }
        void reset_state()
        {
            XUL_DEBUG("reset_state " << xul::make_tuple(m_client_address, m_header_buffer.size(), m_receiving_header, m_handled_requests));
            m_header_buffer.resize(0);
            m_receiving_header = true;
            m_content_length = 0;
            m_received_body_size = 0;
        }

    private:
        XUL_LOGGER_DEFINE();
        http_server_impl& m_owner;
        boost::intrusive_ptr<tcp_socket> m_socket;
        inet_socket_address m_client_address;
        byte_buffer m_header_buffer;
        bool m_receiving_header;
        boost::intrusive_ptr<url_request> m_current_request;
        int m_handled_requests;
        boost::intrusive_ptr<url_session_impl> m_session;
        boost::intrusive_ptr<url_handler> m_handler;
        int64_t m_content_length;
        int64_t m_received_body_size;
    };
    typedef boost::shared_ptr<http_server_client> http_server_client_ptr;
    typedef std::map<http_server_client*, http_server_client_ptr> http_server_client_collection;

    class not_found_handler : public url_handler_adapter
    {
    public:
        virtual void handle_request(url_session* session, const url_request* req)
        {
            url_sessions::send_empty_response(session, 404);
        }
    };

    explicit http_server_impl(io_service* ios)
        : m_options(new http_server_options_impl)
        //, m_tcp_server(new tcp_server_impl(ios))
        , m_io_service(ios)
    {
        XUL_LOGGER_INIT("http_server");
        XUL_DEBUG("new");
        m_acceptor = m_io_service->create_tcp_acceptor();
        m_acceptor->set_listener(this);
        //m_tcp_server->set_listener(this);
        m_empty_router = new http_empty_router;
        set_router(NULL);
    }
    virtual ~http_server_impl()
    {
        XUL_DEBUG("delete");
        m_acceptor->set_listener(NULL);
        stop();
    }

    bool feed_client(tcp_socket* sock)
    {
        return m_acceptor->feed_client(sock);
    }

    virtual void set_router(http_server_router* router)
    {
        m_router = router ? router : m_empty_router.get();
        assert(m_router);
    }
    http_server_router* get_router()
    {
        assert(m_router);
        return m_router.get();
    }
    virtual bool start(int port)
    {
        return m_acceptor->open(port, m_options->enable_reuse_address);
    }
    virtual void stop()
    {
        XUL_DEBUG("stop " << m_clients.size());
        m_acceptor->set_listener(NULL);
        m_acceptor->close();
        while (!m_clients.empty())
        {
            XUL_DEBUG("stop close client " << m_clients.begin()->second);
            m_clients.begin()->second->close();
            m_clients.erase(m_clients.begin());
        }
    }
    virtual void enable_reuse_address(bool enabled)
    {
        m_options->enable_reuse_address = enabled;
    }
    virtual int get_session_count() const
    {
        return m_clients.size();
    }

    io_service* get_io_service()
    {
        return m_io_service.get();
    }
    const http_server_options_impl* get_options() const
    {
        return m_options.get();
    }

    virtual void on_acceptor_client(tcp_acceptor* acceptor, tcp_socket* newClient, const inet_socket_address& sockAddr)
    {
        assert(m_acceptor.get() == acceptor);
        XUL_DEBUG("on_acceptor_client " << sockAddr);
        http_server_client_ptr client(new http_server_client(*this, newClient, sockAddr));
        m_clients[client.get()] = client;
        client->start();
    }
    virtual void on_acceptor_error(tcp_acceptor* acceptor, int errcode)
    {
        XUL_REL_ERROR("on_acceptor_error " << errcode);
    }
    void on_client_error(http_server_client* client, int errcode)
    {
        XUL_ERROR("on_client_error " << errcode);
        client->close();
        m_clients.erase(client);
    }

private:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<io_service> m_io_service;
    boost::intrusive_ptr<tcp_acceptor> m_acceptor;
    http_server_client_collection m_clients;
    //boost::intrusive_ptr<tcp_server_impl> m_tcp_server;
    boost::shared_ptr<http_server_options_impl> m_options;
    boost::intrusive_ptr<http_server_router> m_router;
    boost::intrusive_ptr<http_empty_router> m_empty_router;
};


}
