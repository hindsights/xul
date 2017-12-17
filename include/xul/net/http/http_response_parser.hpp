#pragma once

#include <xul/net/url_response.hpp>
#include <xul/net/http/http_message_parser.hpp>
#include <xul/std/strings.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>


namespace xul {


class http_response_parser : public http_message_parser
{
public:
    /// http_response_parser ready to parse the request method.
    explicit http_response_parser(url_response* req)
    {
        m_response = req;
    }

    const url_response* get_response() const
    {
        return m_response.get();
    }
    url_response* get_response()
    {
        return m_response.get();
    }

    /// Reset to initial parser state.
    void reset()
    {
        http_message_parser::reset();
        m_response->clear();
    }

protected:
    virtual url_message& get_message() { return *m_response; }

    virtual int parse_first_line(const std::string& line)
    {
        std::vector<std::string> parts;
        xul::strings::split_of(parts, line, std::string(" \t"), 3);
        if (parts.size() != 3)
            return error_invalid_status_line;
        int ret = parse_protocol(parts[0]);
        if (ret < 0)
            return ret;
        int statusCode = xul::numerics::parse<int>(parts[1], -1);
        if (statusCode < 0)
            return error_invalid_status_code;
        m_response->set_status_code(statusCode);
        m_response->set_status_description(parts[2].c_str());
        return 0;
    }

private:
    boost::intrusive_ptr<url_response> m_response;
};

}

