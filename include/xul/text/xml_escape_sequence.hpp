#pragma once

#include <xul/text/structured_text_writer.hpp>
#include <xul/std/maps.hpp>
#include <xul/util/singleton.hpp>
#include <xul/io/output_streams.hpp>
#include <xul/io/ostream_stream.hpp>
#include <xul/macro/foreach.hpp>


namespace xul {


class xml_escape_sequence
{
public:
    static void write(output_stream* os, const std::string& val)
    {
        size_t offset = 0;
        for (;;)
        {
            size_t pos = val.find_first_of(escape_table::instance().characters, offset);
            if (std::string::npos == pos)
            {
                if (0 == offset)
                    *os << val;
                else
                    *os << val.substr(offset);
                break;
            }
            *os << val.substr(offset, pos - offset);
            *os << escape_table::instance().escape(val[pos]);
            offset = pos + 1;
        }
    }
    static std::string escape(const std::string& s)
    {
        std::ostringstream oss;
        ostream_stream os(oss);
        write(&os, s);
        return oss.str();
    }

    class escape_table : public singleton<const escape_table>
    {
    public:
        const std::string characters;
        typedef std::map<char, std::string> table_type;
        table_type mapping;

        escape_table() : characters("<>&\'\"")
        {
            mapping['<'] = "&lt;";
            mapping['>'] = "&gt;";
            mapping['&'] = "&amp;";
            mapping['\''] = "&apos;";
            mapping['\"'] = "&quot;";
        }
        std::string escape(char ch) const
        {
            return maps::get(mapping, ch);
        }
    };
};


}


#ifdef XUL_RUN_TEST

#include <xul/util/test_case.hpp>

namespace xul {
class xml_escape_sequence_test_case : public test_case
{
protected:
    virtual void do_run()
    {
        xml_escape_sequence::escape("http://abc/hello?def=1&hp=val#abc") == "http://abc/hello?def=1&amp;hp=val#abc";
        xml_escape_sequence::escape("http://g3.letv.cn/13/12/54/2131450449.0.flv?expect=6&b=1000&tag=box") == "http://g3.letv.cn/13/12/54/2131450449.0.flv?expect=6&amp;b=1000&amp;tag=box";
    }
};
XUL_TEST_SUITE_REGISTRATION(xml_escape_sequence_test_case);
};

#endif
