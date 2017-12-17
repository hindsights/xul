#pragma once

#include <xul/std/strings.hpp>
#include <xul/data/numerics.hpp>
#include <string>
#include <stdint.h>


namespace xul {


class http_content_range
{
public:
    static const char* get_header_name()
    {
        return "Content-Range";
    }

    //std::string range_type;
    int64_t start;
    int64_t end;
    int64_t total_size;

    http_content_range()
    {
        this->reset();
    }

    void reset()
    {
        start = -1;
        end = -1;
        total_size = -1;
    }

    int64_t get_length() const
    {
        if (start < 0 || end < 0)
            return -1;
        return end - start + 1;
    }

    std::string str() const
    {
        return strings::format("%bytes %lld-%lld/%lld", start, end, total_size);
    }

    bool parse(const std::string& s)
    {
        std::pair<std::string, std::string> parts = xul::strings::split_pair(s, ' ');
        if (parts.first.empty() || parts.second.empty())
            return false;
        if (parts.first != "bytes")
            return false;
        std::pair<std::string, std::string> parts2 = xul::strings::split_pair(parts.second, '/');
        total_size = xul::numerics::parse<int64_t>(parts2.second, -1);
        if (parts2.first == "*")
        {
            start = -1;
            end = -1;
            return true;
        }
        std::pair<std::string, std::string> parts3 = xul::strings::split_pair(parts2.second, '-');
        start = xul::numerics::parse<int64_t>(parts3.first, -1);
        end = xul::numerics::parse<int64_t>(parts3.first, -1);
        return true;
    }
};


}
