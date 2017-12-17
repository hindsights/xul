
#include <xul/log/log_manager.hpp>
#include <xul/net/io_service.hpp>
#include <xul/net/http/http_status_code.hpp>
#include <xul/net/url_sessions.hpp>
#include <xul/net/http/http_server.hpp>
#include <xul/net/http/http_server_router.hpp>
#include <xul/net/url_handler_adapter.hpp>
#include <xul/net/url_request.hpp>
#include <xul/net/http/http_content_type.hpp>
#include <xul/lang/object_ptr.hpp>
#include <xul/os/file_system.hpp>
#include <stdio.h>


class hello_handler : public xul::url_handler_adapter
{
public:
    static xul::url_handler* create_instance()
    {
        return new hello_handler;
    }
    hello_handler()
    {
    }

    virtual void handle_request(xul::url_session* session, const xul::url_request* req)
    {
        XUL_APP_REL_INFO("handle_request hello:" << req->get_url());
        xul::url_sessions::send_plain_text(session, xul::http_status_code::ok,
            std::string("hello, world: ") + req->get_uri()->get_path());
    }

    virtual void handle_data_sent(xul::url_session* session, int bytes)
    {
    }
};

class download_file_handler : public xul::url_handler_adapter
{
public:
    static xul::url_handler* create_instance(const std::string& root, const std::string& prefix)
    {
        return new download_file_handler(root, prefix);
    }
    explicit download_file_handler(const std::string& root, const std::string& prefix)
        : m_root(root), m_prefix(prefix)
    {
        m_buffer.resize(16*1024);
    }

    virtual void handle_request(xul::url_session* session, const xul::url_request* req)
    {
        if (req->get_method() != std::string("GET"))
        {
            XUL_APP_REL_ERROR("handle_request download:" << req->get_url());
            xul::url_sessions::send_empty_response(session, xul::http_status_code::not_found);
            return;
        }
        std::string path = req->get_uri()->get_path();
        assert(boost::algorithm::starts_with(path, m_prefix));
        std::string filepath = path.substr(m_prefix.size());
        std::string fullpath = xul::paths::join(m_root, filepath);
        XUL_APP_REL_INFO("handle_request " << xul::make_tuple(path, filepath, fullpath));
        if (false == m_fin.open_binary(fullpath.c_str()))
        {
            xul::url_sessions::send_empty_response(session, xul::http_status_code::not_found);
            return;
        }
        xul::http_range range;
        xul::url_messages::get_range(*req, range);
        XUL_APP_REL_ERROR("handle_request range " << xul::make_tuple(path, filepath, fullpath) << range);
        if (range.start > 0)
        {
            if (false == m_fin.seek(range.start))
            {
                XUL_APP_REL_ERROR("handle_request failed to seek input file " << xul::make_tuple(path, filepath, fullpath) << range);
                xul::url_sessions::send_empty_response(session, xul::http_status_code::request_range_not_satisified);
                return;
            }
        }
        xul::url_messages::set_total_size(*session->get_response(), range, m_fin.get_size());
        xul::url_messages::set_content_type(*session->get_response(), xul::http_content_type::octet_stream());
        session->send_header();
        send_file_data(session);
    }

    virtual void handle_data_sent(xul::url_session* session, int bytes)
    {
        send_file_data(session);
    }
    void send_file_data(xul::url_session* session)
    {
        int size = m_fin.read(m_buffer.data(), m_buffer.size());
        if (size <= 0)
        {
            session->finish();
            return;
        }
        session->send_data(m_buffer.data(), size);
    }

private:
    std::string m_root;
    std::string m_prefix;
    xul::stdfile_reader m_fin;
    xul::byte_buffer m_buffer;
};

class post_file_handler : public xul::url_handler_adapter
{
public:
    static xul::url_handler* create_instance(const std::string& root, const std::string& prefix)
    {
        return new post_file_handler(root, prefix);
    }
    explicit post_file_handler(const std::string& root, const std::string& prefix)
        : m_root(root), m_prefix(prefix), m_received_size(0), m_content_length(-1)
    {
    }

    virtual void handle_request(xul::url_session* session, const xul::url_request* req)
    {
        XUL_APP_REL_ERROR("handle_request upload:" << req->get_url());
        if (req->get_method() != std::string("POST"))
        {
            XUL_APP_REL_ERROR("handle_request upload for non-post:" << req->get_url());
            xul::url_sessions::send_empty_response(session, xul::http_status_code::not_found);
            return;
        }
        std::string path = req->get_uri()->get_path();
        assert(boost::algorithm::starts_with(path, m_prefix));
        std::string filepath = path.substr(m_prefix.size());
        std::string fullpath = xul::paths::join(m_root, filepath);
        std::string dir = xul::paths::get_directory(fullpath);
        if (false == xul::file_system::ensure_directory_exists(dir.c_str()))
        {
            XUL_APP_REL_ERROR("handle_request invlaid path " << xul::make_tuple(path, filepath, fullpath, dir));
            xul::url_sessions::send_empty_response(session, xul::http_status_code::not_found);
            return;
        }
        XUL_APP_REL_INFO("handle_request " << xul::make_tuple(path, filepath, fullpath));
        if (false == m_fout.open_binary(fullpath.c_str()))
        {
            XUL_APP_REL_ERROR("handle_request failed to open file " << xul::make_tuple(path, filepath, fullpath, dir));
            xul::url_sessions::send_empty_response(session, xul::http_status_code::not_found);
            return;
        }
        //? should check content-type and encoding
        m_content_length = xul::url_messages::get_content_length(*req);
        xul::http_content_range contentRange;
        xul::url_messages::get_content_range(*req, contentRange);
        m_content_range = contentRange;
        m_received_size = 0;
        XUL_APP_REL_INFO("handle_request content range " << xul::make_tuple(path, filepath, fullpath) << xul::make_tuple(contentRange.start,  contentRange.end));
        if (m_content_length < 0)
        {
            XUL_APP_REL_WARN("handle_request content length is not specified " << xul::make_tuple(path, filepath, fullpath) << xul::make_tuple(contentRange.start,  contentRange.end));
            xul::url_sessions::send_empty_response(session, xul::http_status_code::bad_request);
            return;
        }
        if (contentRange.start > 0)
        {
            if (false == m_fout.set_position(contentRange.start))
            {
                XUL_APP_REL_ERROR("handle_request failed to seek output file " << xul::make_tuple(path, filepath, fullpath, dir) << xul::make_tuple(contentRange.start,  contentRange.end));
                xul::url_sessions::send_empty_response(session, xul::http_status_code::request_range_not_satisified);
                return;
            }
        }
    }

    virtual void handle_body_data(xul::url_session* session, const uint8_t* data, int size)
    {
        XUL_APP_DEBUG("handle_body_data " << size << " " << m_received_size);
        m_fout.write(data, size);
        m_received_size += size;
        assert(m_received_size <= m_content_length);
        if (m_received_size == m_content_length)
        {
            XUL_APP_REL_EVENT("handle_body_data finished " << xul::make_tuple(m_content_range.start,  m_content_range.end) << m_received_size << " " << m_content_length);
            m_fout.close();
            xul::url_sessions::send_plain_text(session, xul::http_status_code::ok, "The file is uploaded successfully.");
        }
    }

    virtual void handle_data_sent(xul::url_session* session, int bytes)
    {
    }

private:
    std::string m_root;
    std::string m_prefix;
    xul::stdfile_writer m_fout;
    int64_t m_received_size;
    int64_t m_content_length;
    xul::http_content_range m_content_range;
};

int main(int argc, char** argv)
{
    std::string downloadRoot;
    std::string uploadRoot = ".";
    if (argc < 2)
    {
        printf("download directory is not specified.\n");
        printf("USAGE: ./httpsvr DOWNLOAD_DIR [UPLOAD_DIR]\n");
        return 1;
    }
    downloadRoot = argv[1];
    if (false == xul::file_system::directory_exists(downloadRoot.c_str()))
    {
        printf("download directory does not exist: %s\n", downloadRoot.c_str());
        return 2;
    }
    if (argc >= 3)
    {
        uploadRoot = argv[2];
        if (false == xul::file_system::directory_exists(uploadRoot.c_str()))
        {
            printf("upload directory does not exist: %s\n", uploadRoot.c_str());
            return 3;
        }
    }
    boost::intrusive_ptr<xul::log_config> config = xul::create_log_config();
    config->name = "httpsvr";
    config->enable_console_log = true;
    config->enable_file_log = true;
    config->file_append = false;
    config->log_level = xul::LOG_DEBUG;
    xul::log_manager::start_log_service(config.get());
    boost::intrusive_ptr<xul::io_service> ios = xul::create_io_service();
    boost::intrusive_ptr<xul::http_server> server = xul::create_http_server(ios.get());
    boost::intrusive_ptr<xul::simple_http_server_router> router = xul::create_simple_http_server_router();
    router->add("/up/", xul::create_http_server_route_handler(boost::bind(&post_file_handler::create_instance, uploadRoot, "/up/")));
    router->add("/down/", xul::create_http_server_route_handler(boost::bind(&download_file_handler::create_instance, downloadRoot, "/down/")));
    router->add("/hello", xul::create_http_server_route_handler(boost::bind(&hello_handler::create_instance)));
    router->set_not_found_handler(xul::create_http_server_route_handler(boost::bind(&hello_handler::create_instance)));
    server->set_router(router.get());
    server->enable_reuse_address(true);
    server->start(18080);
    ios->start();

    printf("server is started\n");
    getchar();
    server->stop();
    ios->stop();
    return 0;
}

