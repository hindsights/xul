#pragma once

#include <xul/net/http/http_message_parser.hpp>
#include <xul/net/url_request.hpp>
//#include <xul/net/url_encoding.hpp>


namespace xul {


class http_request_parser : public http_message_parser
{
public:
    /// http_response_parser ready to parse the request method.
    explicit http_request_parser(url_request* req)
    {
        m_request = req;
    }

    const url_request* get_request() const
    {
        return m_request.get();
    }
    url_request* get_request()
    {
        return m_request.get();
    }

    /// Reset to initial parser state.
    void reset()
    {
        http_message_parser::reset();
        m_request->clear();
    }

protected:
    virtual url_message& get_message() { return *m_request; }

    virtual int parse_first_line(const std::string& line)
    {
        std::vector<std::string> parts;
        xul::strings::split_of(parts, line, std::string(" \t"));
        if (parts.size() != 3)
            return error_invalid_request_line;
        int ret = parse_protocol(parts[2]);
        if (ret < 0)
            return ret;
        m_request->set_method(parts[0].c_str());
        std::string urlstr = parts[1];
        m_request->set_url(urlstr.c_str());
        return 0;
    }

private:
    boost::intrusive_ptr<url_request> m_request;
};


}
