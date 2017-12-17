#pragma once

#include <xul/std/maps.hpp>
#include <string>
#include <map>


namespace xul {


class http_status_code
{
public:
    enum
    {
        none = 0,
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        partial_content = 206,
        multiple_choices = 300,
        moved_permanently = 301,
        found = 302,
        moved_temporarily = 302,
        not_modified = 304,
        temporary_redirect = 307,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        request_range_not_satisified = 416,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    };

    class status_code_table
    {
    public:
        std::map<int, std::string> table;
        status_code_table()
        {
            table[ok] = "OK";
            table[created] = "Created";
            table[accepted] = "Accepted";
            table[no_content] = "No Content";
            table[partial_content] = "Partial Content";
            table[multiple_choices] = "Multiple Choices";
            table[moved_permanently] = "Moved Permanently";
            table[found] = "Found";
            table[not_modified] = "Not Modified";
            table[temporary_redirect] = "Temporary Redirect";
            table[bad_request] = "Bad Request";
            table[unauthorized] = "Unauthorized";
            table[forbidden] = "Forbidden";
            table[not_found] = "Not Found";
            table[internal_server_error] = "Internal Server Error";
            table[not_implemented] = "Not Implemented";
            table[bad_gateway] = "Bad Gateway";
            table[service_unavailable] = "Service Unavailable";
            table[request_range_not_satisified] = "Requested Range Not Satisfiable";
        }

        std::string get_description(int code) const
        {
            static const std::string emptystr;
            return xul::maps::get(table, code, emptystr);
        }
    };

    static std::string get_description(int code)
    {
        static status_code_table table;
        return table.get_description(code);
    }
};


}
