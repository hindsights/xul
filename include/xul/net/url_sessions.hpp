#pragma once

#include <xul/net/url_session.hpp>
#include <xul/net/url_messages.hpp>
#include <xul/net/http/http_content_type.hpp>


namespace xul {


/// utility functions
class url_sessions
{
public:
    static bool send_empty_response(url_session* session, int status_code)
    {
        session->set_chunked(false);
        session->get_response()->set_status_code(status_code);
        session->get_response()->set_header("Content-Length", "0");
        if (!session->send_header())
            return false;
        session->finish();
        return true;
    }

    static bool send_plain_text(url_session* session, int status_code, const std::string& content)
    {
        return send_simple_response(session, status_code, content, http_content_type::plain_text());
    }
    static bool send_simple_response(url_session* session, int status_code, const std::string& content, const char* content_type)
    {
        session->set_chunked(false);
        url_messages::init_response(*session->get_response(), status_code, content.size(), content_type);
        if (!session->send_header())
            return false;
        if (!content.empty() && !session->send_data((const uint8_t*)content.data(), content.size()))
            return false;
        session->finish();
        return true;
    }
    static bool redirect_permanently(url_session* session, const char* url)
    {
        return do_redirect(session, url, http_status_code::moved_permanently);
    }
    static bool redirect_temporarily(url_session* session, const char* url)
    {
        return do_redirect(session, url, http_status_code::moved_temporarily);
    }

private:
    static bool do_redirect(url_session* session, const char* url, int status_code)
    {
        session->get_response()->set_header("Location", url);
        session->get_response()->set_header("Connection", "close");
        return send_empty_response(session, status_code);
    }
};


}
