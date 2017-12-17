#pragma once

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <list>
#include <iterator>
#include <map>
#include <assert.h>


namespace xul {


class http_header
{
public:
    typedef std::map<std::string, std::string> string_table;
    string_table fields;

    static boost::regex& crlf_regex()
    {
        static boost::regex crlf("\r\n");
        return crlf;
    }

    http_header()
    {

    }

    bool parse(const std::string& s)
    {
        using std::list;
        using std::string;
        list<string> lines;
        string delim = "\r\n";
        boost::sregex_token_iterator iter(s.begin(), s.end(), crlf_regex(), -1);
        boost::sregex_token_iterator endIter;
        for ( ; iter != endIter; ++iter )
        {
            const string& line = *iter;
            if (line.empty())
                continue;
            lines.push_back(line);
        }
        if (lines.empty())
            return false;
        string firstLine = lines.front();
        lines.pop_front();
        fields.clear();
        BOOST_FOREACH(const string& line, lines)
        {
            size_t pos = line.find(':');
            if (string::npos == pos)
            {
                assert(false);
                continue;
            }
            string name = line.substr(0, pos);
            string val = line.substr(pos + 1);
            boost::trim(name);
            boost::trim(val);
            assert(fields.find(name) == fields.end());
            fields[name] = val;
        }
        return true;
    }
};

}


