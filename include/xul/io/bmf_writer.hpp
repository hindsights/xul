#pragma once

/**
 * @file
 * @brief bmf(binary message format)
 */

#include <xul/io/bmf_format.hpp>
#include <xul/io/bmf_integer_encoding.hpp>
#include <xul/io/output_stream.hpp>
#include <boost/noncopyable.hpp>
#include <stddef.h>
#include <stdint.h>


namespace xul {


/// bmf(binary message format)
class bmf_writer : boost::noncopyable, public bmf_format
{
public:
    explicit bmf_writer(output_stream& os) : m_os(os)
    {
    }
    virtual ~bmf_writer()
    {
    }

    void write_string_element(const std::string& key, const std::string& val)
    {
        write_key(key);
        write_string(val);
    }

    void write_integer_element(const std::string& key, int64_t val)
    {
        write_key(key);
        write_integer(val);
    }

    void write_integer(int64_t val)
    {
        m_os.write_byte(integer_tag);
        bmf_integer_encoding::encode(val, m_os);
    }

    void write_string(const std::string& val)
    {
        start_bytes(val.size());
        m_os.write_chars(val.data(), val.size());
    }

    void write_key(const std::string& val)
    {
        write_string(val);
    }

    void start_bytes(size_t size)
    {
        m_os.write_byte(string_tag);
        bmf_integer_encoding::encode(size, m_os);
    }
    void write_bytes(const uint8_t* data, size_t size)
    {
        start_bytes(size);
        append_bytes(data, size);
    }
    void append_bytes(const uint8_t* data, size_t size)
    {
        m_os.write_bytes(data, size);
    }
    void append_string(const char* data, size_t size)
    {
        m_os.write_bytes((const uint8_t*)data, size);
    }

    ///
    void start_list()
    {
        m_os.write_byte(list_tag);
    }
    void end_list()
    {
        m_os.write_byte(end_tag);
    }
    void start_dict()
    {
        m_os.write_byte(dict_tag);
    }
    void end_dict()
    {
        m_os.write_byte(end_tag);
    }

private:
    output_stream& m_os;
};


}
