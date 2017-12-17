#pragma once

#include <xul/net/http/detail/inner_http_connection.hpp>
#include <xul/net/http/detail/http_filter.hpp>
#include <xul/net/http/http_response_decoder.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/macro/iterate.hpp>


namespace xul { 


http_filter* create_gzip_http_filter();
http_filter* create_chunked_http_filter();


namespace detail {


class http_connection_request_info : public object_impl<object>
{
public:
    class end_filter : public object_impl<http_filter>
    {
        xul::logger* get_xul_logger() const { return m_xul_logger.get(); }
        const void* get_xul_logger_owner() const { return m_owner; }
        mutable boost::intrusive_ptr<xul::logger> m_xul_logger;
        inner_http_connection* const m_owner;
        int64_t m_content_length;
        int64_t m_received_size;
        bool m_finished;
    public:
        explicit end_filter(inner_http_connection& owner) : m_owner(&owner)
        {
            m_xul_logger = owner.get_logger();
            m_received_size = 0;
            m_content_length = -1;
            m_finished = false;
        }
        bool check_finish(url_request* req, url_response* resp, const uint8_t* data, int size)
        {
            if (m_content_length >= 0 && m_received_size >= m_content_length)
            {
                assert(m_content_length == m_received_size);
                return finish(req, resp, data, size);
            }
            return true;
        }
        virtual const char* name() const { return "end"; }

        virtual bool init(url_request* req, url_response* resp)
        {
            m_content_length = url_messages::get_content_length(*resp);
            m_received_size = 0;
            XUL_INFO("init " << xul::make_tuple(m_content_length, m_received_size));
            m_finished = false;
            if (!m_owner->on_header(req, resp))
                return false;
            return check_finish(req, resp, NULL, 0);
        }
        virtual bool finish(url_request* req, url_response* resp, const uint8_t* data, int size)
        {
            XUL_INFO("finish " << xul::make_tuple(size, m_content_length, m_received_size));
            if (m_finished)
            {
                XUL_WARN("end_http_filter.finish: already finished " << xul::make_tuple(this, static_cast<const void*>(data), size, m_finished));
                m_owner->abort_handling();
                return false;
            }
            m_finished = true;
            return m_owner->on_complete(req, resp, m_received_size, data, size);
        }
        virtual void abort(url_request* req, url_response* resp)
        {
            XUL_WARN("abort " << this << " " << m_finished);
            m_owner->abort_handling();
        }
        virtual bool process(url_request* req, url_response* resp, const uint8_t* data, int size)
        {
            XUL_INFO("process " << xul::make_tuple(size, m_content_length, m_received_size));
            assert(m_content_length < 0 || m_received_size < m_content_length);
            int realsize = size;
            if (m_content_length >= 0 && m_received_size + size > m_content_length)
                realsize = m_content_length - m_received_size;
            m_received_size += realsize;
            if (realsize > 0)
            {
                if (!m_owner->on_content(req, resp, data, realsize))
                    return false;
            }
            check_finish(req, resp, data + realsize, size - realsize);
            return true;
        }
        virtual void handle_error(url_request* req, url_response* resp, int errcode)
        {
            XUL_INFO("handle_error " << xul::make_tuple(errcode, m_content_length, m_received_size));
            m_owner->on_error(errcode);
        }
        virtual void set_next(http_filter* next_filter)
        {
            assert(false);
        }
    };

public:
    const boost::intrusive_ptr<http_filter> endfilter;
    boost::intrusive_ptr<uri> url;
    boost::intrusive_ptr<url_request> request;
    boost::shared_ptr<http_response_extractor> response_extractor;
    boost::intrusive_ptr<http_filter> filters;
    bool response_header_received;
    bool header_send;

    explicit http_connection_request_info(inner_http_connection* conn, uri* u) 
        : url(u)
        , request(create_url_request())
        , response_extractor(new http_response_extractor)
        , filters(endfilter)
        , endfilter(new end_filter(*conn))
        , response_header_received(false)
        , m_connection(conn)
        , m_redirect_times(0)
        , header_send(false)
    {
        m_xul_logger = conn->get_logger();
        XUL_DEBUG("new request_info " << this);
    }

    virtual ~http_connection_request_info()
    {
        XUL_DEBUG("delete request_info " << this);
    }

    void init_request_host()
    {
        std::string host = url->get_host();
        if (url->get_port() != 80 && url->get_port() != 0)
            host = host + ":" + xul::numerics::format<int>(url->get_port());
        request->set_header("Host", host.c_str());
    }

    void prepare_request(const string_table* headers)
    {
        const http_connection_options* opts = m_connection->get_options();
        request->set_protocol_name(opts->get_protocol_name());
        request->set_protocol_version(opts->get_protocol_version());
#if 1
        if (headers)
        {
            //for (boost::intrusive_ptr<iterator<const string_table_entry> > iter = m_headers->iterate(); iter->available(); iter->next())
            XUL_ITERATE(string_table, headers, iter)
            {
                const string_table::entry_type* item = iter->element();
#if 0
                if (xul::strings::iequals("User-Agent", item->get_key())
                    || xul::strings::iequals("Pragma", item->get_key()) 
                    || xul::strings::iequals("Cookie", item->get_key()) 
                    || xul::strings::iequals("Authorization", item->get_key())
                    )
#endif
                {
                    request->set_header(item->get_key(), item->get_value());
                }
            }
        }
#endif
        if (!request->get_headers()->contains("Host"))
        {
            init_request_host();
        }
        if (!request->get_headers()->contains("User-Agent"))
        {
            request->set_header("User-Agent", "Mozilla/5.0");
        }
        //req.set_header("Accept", "*/*");
        assert(!std::string(m_connection->get_options()->get_protocol_version()).empty());
        request->set_protocol_version(m_connection->get_options()->get_protocol_version());
        if (opts->is_keep_alive())
        {
            request->set_header("Connection", "keep-alive");
        }
        else
        {
            request->set_header("Connection", "close");
        }
        //if (m_range.is_valid())
        //    url_messages::set_range(*req, m_range);
        //req->set_method("GET");
        request->set_url(url->get_full_path());
        if (opts->is_gzip_enabled())
        {
            request->set_header("Accept-Encoding", "gzip");
        }
    }

    void reset_handling_state()
    {
        response_header_received = false;
        response_extractor->reset();
        filters = this->endfilter;
        header_send = false;
    }
    void add_filter(http_filter* filter)
    {
        filter->set_next(this->filters.get());
        this->filters = filter;
    }
    bool process(const uint8_t* data, size_t size)
    {
        if (response_extractor->is_header_finished())
        {
            return process_body_data(data, size);
        }
        else
        {
            return process_header_data(data, size);
        }
    }
    bool process_body_data(const uint8_t* data, size_t size)
    {
        return filters->process(request.get(), response_extractor->get_response(), data, size);
    }
    bool process_header_data(const uint8_t* data, size_t size)
    {
        int consumed_size = response_extractor->feed(data, size);
        if (consumed_size < 0)
        {
            XUL_DEBUG("failed to parse http response " << consumed_size);
            assert(false);
            m_connection->handle_error(http_error_codes::invalid_response_header);
            return false;
        }
        if (response_extractor->is_header_finished())
        {
            if (!process_raw_response())
                return false;
            if (size > consumed_size)
            {
                if (!m_connection->is_started())
                {
                    m_connection->abort_handling();
                    return false;
                }
                return process_body_data(data + consumed_size, size - consumed_size);
            }
        }
        return true;
    }
    bool process_raw_response()
    {
        XUL_DEBUG("process_raw_response");
        url_response* resp = response_extractor->get_response();
        int statusCode = resp->get_status_code();
        if ((statusCode >= 300 && statusCode <= 303) || 307 == statusCode)
        {
            std::string location = resp->get_header("Location", "");
            redirect(location, statusCode);
            return false;
        }
        return handle_response_header();
    }
    bool redirect(const std::string& location, int statusCode)
    {
        // handle redirection
        if (m_redirect_times > m_connection->get_options()->get_max_redirects())
        {
            XUL_REL_ERROR("redirect too many times " 
                << xul::make_tuple(statusCode, m_redirect_times, m_connection->get_options()->get_max_redirects()) 
                << " " << location);
            m_connection->handle_error(http_error_codes::redirect_too_many_times);
            return false;
        }
        XUL_INFO("redirect to " << statusCode << " " << location);
        m_redirect_times++;
        boost::intrusive_ptr<uri> locationUri = create_uri();
        if (location.empty() || !locationUri->parse(location.c_str()))
        {
            assert(false);
            m_connection->handle_error(http_error_codes::invalid_redirect_url);
            return false;
        }
        reset_handling_state();
        url->parse(location.c_str());
        request->set_url(locationUri->get_full_path());
        init_request_host();
        return m_connection->redirect(this);
    }
    bool handle_response_header()
    {
        //reqInfo.response = resp;
        //m_state.received_size = 0;
        response_header_received = true;
        init_filters();
        return filters->init(request.get(), response_extractor->get_response());
    }
    void init_filters()
    {
        filters = new end_filter(*m_connection);
        std::string contentEncoding = response_extractor->get_response()->get_header("Content-Encoding", "");
        xul::strings::trim(contentEncoding);
        if (contentEncoding == "gzip")
        {
            add_filter(create_gzip_http_filter());
        }
#if 1
        std::string transferEncoding = response_extractor->get_response()->get_header("Transfer-Encoding", "");
        xul::strings::trim(transferEncoding);
        if (transferEncoding == "chunked")
        {
            add_filter(create_chunked_http_filter());
        }
#endif
    }

private:
    xul::logger* get_xul_logger() const { return m_xul_logger.get(); }
    const void* get_xul_logger_owner() const { return this->m_connection; }
    mutable boost::intrusive_ptr<xul::logger> m_xul_logger;
    int m_redirect_times;
    inner_http_connection* const m_connection;
};

typedef boost::intrusive_ptr<http_connection_request_info> http_connection_request_info_ptr;


} }
