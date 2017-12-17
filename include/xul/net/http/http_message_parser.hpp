#pragma once

#include <xul/net/http/http_message.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>


namespace xul {


class http_message_parser
{
public:
    static const char* find_header_end(const char* data, size_t size)
    {
        const char* header_end_tag = "\r\n\r\n";
        const size_t header_end_tag_size = 4;
        const char* pos = std::search(data, data + size, header_end_tag, header_end_tag + header_end_tag_size);
        if (pos == data + size)
            return NULL;
        return pos;
    }

    static const char* find_line_end(const char* data, size_t size)
    {
        const char* line_tag = "\r\n";
        const size_t line_tag_size = 2;
        const char* pos = std::search(data, data + size, line_tag, line_tag + line_tag_size);
        if (pos == data + size)
        {
            return NULL;
        }
        return pos;
    }

public:
    enum
    {
        error_invalid_header_line = -1,
        error_invalid_protocol_part = -2,
        error_invalid_status_line = -3,
        error_invalid_status_code = -4,
        error_invalid_request_line = -5,
    };

    /// http_response_parser ready to parse the request method.
    http_message_parser()
    {
        reset();
    }

    /// Reset to initial parser state.
    void reset()
    {
        m_first_line = true;
    }

    bool parse(const char* data, size_t size)
    {
        m_first_line = true;
        for (;;)
        {
            const char* pos = find_line_end(data, size);
            if (NULL == pos)
                break;
            assert(pos >= data);
            size_t lineLen = pos - data;
            int ret = parse_line(data, lineLen);
            if (ret > 0)
                return true;
            if (ret < 0)
                break;
            data = pos + 2;
        }
        return false;
    }

protected:
    virtual int parse_first_line(const std::string& line) = 0;
    virtual url_message& get_message() = 0;

    int parse_line(const char* linebuf, size_t size)
    {
        return parse_line(std::string(linebuf, size));
    }
    int parse_line(const std::string& line)
    {
        if (m_first_line)
        {
            int ret = parse_first_line(line);
            m_first_line = false;
            return ret;
        }
        return parse_header(line);
    }

    int parse_header(const std::string& line)
    {
        if (line.empty())
            return 1;
        std::pair<std::string, std::string> keyvals = xul::strings::split_pair(line, ':');
        if (keyvals.first.empty() || keyvals.first.size() == line.size())
            return error_invalid_header_line;
        get_message().set_header(boost::algorithm::trim_copy(keyvals.first).c_str(), boost::algorithm::trim_copy(keyvals.second).c_str());
        return 0;
    }

    int parse_protocol(const std::string& s)
    {
        std::vector<std::string> parts;
        boost::algorithm::split(parts, s, boost::algorithm::is_any_of("/"), boost::algorithm::token_compress_on);
        if (parts.size() != 2)
            return error_invalid_protocol_part;
        get_message().set_protocol_name(parts[0].c_str());
        get_message().set_protocol_version(parts[1].c_str());
        return 0;
    }

private:
    bool m_first_line;

};

}