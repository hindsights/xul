#pragma once

#include <xul/net/http/http_connection.hpp>
#include <xul/lang/object_ptr.hpp>


namespace xul {


class http_connections
{
public:
    static bool download(http_connection* conn, const char* urlstr, const string_table* headers)
    {
        return conn->request(XUL_HTTP_GET, urlstr, headers);
    }
    static bool download(http_connection* conn, const char* urlstr, int64_t pos, int64_t len, const string_table* headers)
    {
        assert(len != 0);
        boost::intrusive_ptr<xul::string_table> new_headers = headers->clone();
        http_range range;
        range.start = pos;
        if (pos >= 0 && len >= 0)
            range.end = pos + len - 1;
        return download(conn, urlstr, range, headers);
    }
    static bool download(http_connection* conn, const char* urlstr, const http_range& range, const string_table* headers)
    {
        boost::intrusive_ptr<xul::string_table> new_headers = headers->clone();
        if (range.is_valid())
            new_headers->set(http_range::get_header_name(), range.str().c_str());
        return download(conn, urlstr, new_headers.get());
    }
};


}
