#pragma once

#include <xul/net/url_request.hpp>
#include <xul/net/url_response.hpp>
#include <xul/net/http/http_range.hpp>
#include <xul/net/http/http_status_code.hpp>
#include <xul/net/http/http_content_range.hpp>
#include <time.h>

namespace xul {


/// utility functions for url_message
class url_messages
{
public:
    static std::string get_user_agent(const url_message& msg)
    {
        return msg.get_header("User-Agent", "");
    }
    static int64_t get_content_length(const url_message& msg)
    {
        const char* str = msg.get_header("Content-Length", "");
        return str ? numerics::parse<int64_t>(str, -1) : -1;
    }

    static bool get_range(const url_message& msg, http_range& range)
    {
        return range.parse(msg.get_header("Range", ""));
    }

    static std::string get_content_type(const url_message& msg)
    {
        return msg.get_header("Content-Type", "");
    }

    static bool is_chunked(const url_message& msg)
    {
        const char *str = msg.get_header("Transfer-Encoding", "");
        if (strlen(str) && strcmp(str,"chunked") == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool get_content_range(const url_message& msg, http_content_range& range)
    {
        http_content_range temp_range;
        const char* str = msg.get_header(temp_range.get_header_name(), "");
        if (!str || !temp_range.parse(str))
            return false;
        range = temp_range;
        return true;
    }
    static int64_t get_total_size(const url_message* msg)
    {
        int64_t content_length = get_content_length(*msg);
        int64_t total_size = content_length;
        http_content_range range;
        if (get_content_range(*msg, range) && range.total_size >= 0)
        {
            total_size = range.total_size;
        }
        return total_size;
    }

    static void set_chunked(url_message& msg)
    {
        msg.set_protocol_version("1.1");
        msg.set_header("Transfer-Encoding", "chunked");
    }

    static void set_content_length(url_message& msg, int64_t content_length)
    {
        msg.set_header("Content-Length", numerics::format<int64_t>(content_length).c_str());
    }
    static void set_content_type(url_message& msg, const char* content_type)
    {
        msg.set_header("Content-Type", content_type);
    }
    static void init_response(url_response& resp, int64_t content_length, const char* content_type)
    {
        if (content_length >= 0)
            set_content_length(resp, content_length);
        set_content_type(resp, content_type);
    }
    static void init_response(url_response& resp, int status_code, int64_t content_length, const char* content_type)
    {
        resp.set_status_code(status_code);
        init_response(resp, content_length, content_type);
    }

    static void set_date(url_message& msg)
    {
        char tempbuf[1024];
        time_t secs = time(NULL);
        tm* t = gmtime(&secs);
        strftime(tempbuf, 1024, "%a, %b %d %Y %H:%M:%S GMT", t);
        msg.set_header("Date", tempbuf);
    }

    static void set_content_range(url_message& msg, const http_content_range& range)
    {
        assert(range.end > 0);
        msg.set_header(http_content_range::get_header_name(), range.str().c_str());
    }

    static void set_range(url_message& msg, int64_t pos, int64_t len)
    {
        http_range range(pos, -1);
        if (len > 0)
            range.end = range.start + len - 1;
        set_range(msg, range);
    }
    static void set_range(url_message& msg, const http_range& range)
    {
        msg.set_header(http_range::get_header_name(), range.str().c_str());
    }

    static int64_t set_total_size(url_response& resp, const http_range& request_range, int64_t total_size)
    {
        int64_t content_length = -1;
        if (request_range.is_valid())
        {
            if (total_size < 0 && request_range.start <= 0)
            {
                resp.set_status_code(http_status_code::ok);
                content_length = total_size;
            }
            else if (request_range.start >= total_size)
            {
                resp.set_status_code(http_status_code::request_range_not_satisified);
                resp.set_header(http_content_range::get_header_name(), xul::strings::format("bytes */%d", total_size).c_str());
                content_length = 0;
            }
            else
            {
                resp.set_status_code(http_status_code::partial_content);
                if (request_range.get_length() >= 0 && request_range.end < total_size)
                    content_length = request_range.get_length();
                else
                    content_length = total_size - request_range.get_real_start();
                if (request_range.start >= 0 || request_range.end >= 0)
                {
                    assert(request_range.start >= 0);
                    http_content_range contentRange;
                    contentRange.start = request_range.start;
                    contentRange.end = request_range.end;
                    contentRange.total_size = total_size;
                    if (contentRange.end < 0)
                    {
                        contentRange.end = contentRange.total_size - 1;
                    }
                    assert(contentRange.end >= 0);
                    set_content_range(resp, contentRange);
                }
            }
        }
        else
        {
            resp.set_status_code(http_status_code::ok);
            content_length = total_size;
        }
        if (content_length >= 0)
            set_content_length(resp, content_length);
        return content_length;
    }

};


}
