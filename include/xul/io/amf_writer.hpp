#pragma once

/**
 * @file
 * @brief bmf(binary message format)
 */

#include <xul/io/structured_writer.hpp>
#include <xul/io/amf_types.hpp>
#include <xul/io/output_stream.hpp>
#include <xul/io/data_output_stream.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/noncopyable.hpp>
#include <stddef.h>
#include <stdint.h>


namespace xul {


/// bmf(binary message format)
class amf_writer : public xul::object
{
public:
    virtual void write_null() = 0;
};


class amf0_writer : public xul::object_impl<amf_writer>, public amf0_types
{
public:
    explicit amf0_writer(data_output_stream& os) : m_os(os)
    {
    }
    virtual ~amf0_writer()
    {
    }
    virtual void write_null()
    {
        m_os.write_uint8(amf0_null);
    }

    virtual void write_undefined()
    {
        m_os.write_uint8(amf0_undefined);
    }

    void write_null_element(const std::string& key)
    {
        write_key(key);
        write_null();
    }

    void write_undefined_element(const std::string& key)
    {
        write_key(key);
        write_undefined();
    }

    void write_string_element(const std::string& key, const std::string& val)
    {
        write_key(key);
        write_string(val);
    }

    void write_number_element(const std::string& key, int64_t val)
    {
        write_key(key);
        write_number(val);
    }

    void write_bool_element(const std::string& key, bool val)
    {
        write_key(key);
        write_bool(val);
    }

    void write_bool(bool val)
    {
        m_os.write_byte(amf0_boolean);
        m_os.write_uint8(val ? 1 : 0);
    }

    void write_number(double val)
    {
        m_os.write_byte(amf0_number);
        m_os.write_double(val);
    }

    void write_string(const std::string& val)
    {
        start_bytes(val.size());
        m_os.write_bytes(val.data(), val.size());
    }

    void write_key(const std::string& val)
    {
        m_os.write_uint16(val.size());
        m_os.write_string(val);
    }

    void start_bytes(size_t size)
    {
        m_os.write_byte(amf0_string);
        m_os.write_uint16(size);
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

    virtual void start_scalar(const char* name)
    {
        size_t len = strlen(name);
        m_os.write_uint16(len);
        m_os.write_raw_buffer(name, len);
    }
    virtual void end_scalar(const char* name)
    {
    }

    ///
    void start_list()
    {
        m_os.write_byte(amf0_array);
    }
    void end_list()
    {
    }
    void start_dict()
    {
        m_os.write_byte(amf0_object);
    }
    void end_dict()
    {
        m_os.write_uint8(0);
        m_os.write_uint8(0);
        m_os.write_byte(amf0_end_of_object);
    }

private:
    data_output_stream& m_os;
};


}
