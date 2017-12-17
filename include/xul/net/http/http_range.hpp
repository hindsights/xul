#pragma once

#include <xul/std/strings.hpp>
#include <xul/data/numerics.hpp>
#include <stdint.h>


namespace xul {


class http_range
{
public:
    static const char* get_header_name()
    {
        return "Range";
    }

    //std::string range_type;
    int64_t start;
    int64_t end;

    explicit http_range(int64_t x, int64_t y) : start(x), end(y)
    {
        assert(x == -1 || x >= 0);
        assert(y == -1 || (y >= 0 && x >= 0 && x <= y));
    }
    http_range()
    {
        clear();
    }

    bool is_valid() const
    {
        return (start >= 0 || end >= 0);
    }

    int64_t get_length() const
    {
        if (start < 0 || end < 0)
            return -1;
        return end - start + 1;
    }
    int64_t get_real_start() const
    {
        return start >= 0 ? start : 0;
    }

    void clear()
    {
        //range_type = "bytes";
        start = -1;
        end = -1;
    }
    std::string str() const
    {
        if (start < 0)
        {
            if (end < 0)
                return "bytes=-";
            return strings::format("bytes=-%lld", end);
        }
        if (end < 0)
            return strings::format("bytes=%lld-", start);
        return strings::format("bytes=%lld-%lld", start, end);
    }
    bool parse(const std::string& s)
    {
        std::pair<std::string, std::string> parts = strings::split_pair(s, '=');
        if (parts.first.empty() || parts.second.empty())
            return false;
        if (parts.first != "bytes")
            return false;
        std::pair<std::string, std::string> parts2 = strings::split_pair(parts.second, '-');
        //if (parts2.first.empty() || parts2.second.empty())
        //    return false;
        //range_type = parts.first;
        start = numerics::parse<int64_t>(parts2.first, -1);
        end = numerics::parse<int64_t>(parts2.second, -1);
        return true;
    }
};


inline std::ostream& operator<<(std::ostream& os, const http_range& range)
{
    return os << "http_range:" << range.str();
}


}
