#pragma once

#include <xul/io/output_streams.hpp>
#include <xul/std/maps.hpp>
#include <xul/data/numerics.hpp>
#include <xul/data/string_table.hpp>
#include <xul/data/map.hpp>
#include <xul/macro/iterate.hpp>


namespace xul {


class http_message
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

public:
    http_message() : m_headers(create_associative_istring_array())
    {
    }
    ~http_message()
    {
    }

    void write_headers(output_stream& os) const
    {
        XUL_ITERATE(string_table, m_headers, iter)
        {
            const string_table::entry_type* entry = iter->element();
            assert(entry);
            os << entry->get_key() << ": " << entry->get_value() << "\r\n";
        }
        os.write_string("\r\n");
    }

    void clear()
    {
        m_headers->clear();
        m_protocol_name = "HTTP";
        m_protocol_version = "1.0";
    }

    const char* get_protocol_name() const { return m_protocol_name.c_str(); }
    void set_protocol_name(const char* name) { m_protocol_name = name; }

    const char* get_protocol_version() const { return m_protocol_version.c_str(); }
    void set_protocol_version(const char* val)
    {
        m_protocol_version = val;
    }

    const char* get_header(const char* name, const char* default_val) const
    {
        return m_headers->get(name, "");
    }
    void set_header(const char* name, const char* val)
    {
        m_headers->set(name, val);
    }

    const string_table* get_headers() const { return m_headers.get(); }
    string_table* ref_headers() { return m_headers.get(); }
    void set_headers(const string_table* header)
    {
        if (header)
            m_headers = header->clone();
    }

protected:
    //typedef std::map<std::string, std::string, xul::string_iless<char> > header_collection_type;
    //header_collection_type m_headers;
    boost::intrusive_ptr<string_table> m_headers;
    std::string m_protocol_name;
    std::string m_protocol_version;
};

}
