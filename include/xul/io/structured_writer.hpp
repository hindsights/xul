#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/variant.hpp>
#include <string>
#include <sstream>
#include <stdint.h>


namespace xul {


class variant;
class output_stream;


/// interface for xml/json/yml/bencode
class structured_writer : public object
{
public:
    virtual void start_scalar(const char* name) = 0;
    virtual void end_scalar(const char* name) = 0;
    virtual void start_list(const char* name, int count) = 0;
    virtual void end_list() = 0;
    virtual void start_dict(const char* name) = 0;
    virtual void end_dict() = 0;

    virtual void write_bool_value(bool val) = 0;
    virtual void write_int_value(int val) = 0;
    virtual void write_uint_value(unsigned int val) = 0;
    virtual void write_int64_value(int64_t val) = 0;
    virtual void write_uint64_value(uint64_t val) = 0;
    virtual void write_float_value(float val) = 0;
    virtual void write_double_value(double val) = 0;
    virtual void write_long_double_value(long double val) = 0;
    virtual void write_string_value(const char* val) = 0;
    virtual void write_simple_value(const char* val) = 0;
    //virtual void write_bool_value(bool val) = 0;

    virtual void write(const variant* var) = 0;
    virtual void write_list(const variant_list* vars) = 0;
    virtual void write_dict(const variant_dict* vars) = 0;


    /// the following are utility functions

    void write_bool(const char* name, bool val)
    {
        start_scalar(name);
        write_bool_value(val);
        end_scalar(name);
    }

    void write_integer(const char* name, int val)
    {
        start_scalar(name);
        write_int_value(val);
        end_scalar(name);
    }
    void write_integer(const char* name, unsigned int val)
    {
        start_scalar(name);
        write_uint_value(val);
        end_scalar(name);
    }
    void write_integer(const char* name, long long val)
    {
        start_scalar(name);
        write_int64_value(val);
        end_scalar(name);
    }
    void write_integer(const char* name, unsigned long long val)
    {
        start_scalar(name);
        write_uint64_value(val);
        end_scalar(name);
    }
    void write_integer(const char* name, long val)
    {
        start_scalar(name);
        write_int64_value(val);
        end_scalar(name);
    }
    void write_integer(const char* name, unsigned long val)
    {
        start_scalar(name);
        write_uint64_value(val);
        end_scalar(name);
    }
    void write_number(const char* name, long val) { write_integer(name, val); }
    void write_number(const char* name, unsigned long val) { write_integer(name, val); }
    void write_number(const char* name, int val) { write_integer(name, val); }
    void write_number(const char* name, unsigned int val) { write_integer(name, val); }
    void write_number(const char* name, long long val) { write_integer(name, val); }
    void write_number(const char* name, unsigned long long val) { write_integer(name, val); }

    void write_float(const char* name, float val)
    {
        start_scalar(name);
        write_float_value(val);
        end_scalar(name);
    }
    void write_float(const char* name, double val)
    {
        start_scalar(name);
        write_double_value(val);
        end_scalar(name);
    }
    void write_float(const char* name, long double val)
    {
        start_scalar(name);
        write_long_double_value(val);
        end_scalar(name);
    }
    void write_number(const char* name, float val) { write_float(name, val); }
    void write_number(const char* name, double val) { write_float(name, val); }
    void write_number(const char* name, long double val) { write_float(name, val); }

    void write_simple(const char* name, const char* val)
    {
        start_scalar(name);
        write_simple_value(val);
        end_scalar(name);
    }
    void write_simple(const char* name, const std::string& val)
    {
        write_simple(name, val.c_str());
    }
    template <typename T>
    void write_simple(const char* name, const T& val)
    {
        std::ostringstream os;
        os << val;
        write_simple(name, os.str());
    }
    template <typename T>
    void write_simple(const T& val)
    {
        std::ostringstream os;
        os << val;
        write_simple_value(os.str());
    }
    template <typename T>
    void write_simple_value(const T& val)
    {
        std::ostringstream os;
        os << val;
        write_simple_value(os.str());
    }

    void write_string(const char* name, const char* val)
    {
        start_scalar(name);
        write_string_value(val);
        end_scalar(name);
    }
    void write_string(const char* name, const std::string& val)
    {
        write_string(name, val.c_str());
    }
    template <typename T>
    void write_string(const char* name, const T& val)
    {
        std::ostringstream os;
        os << val;
        write_string(name, os.str());
    }

    void write(const char* name, const char* val) { write_string(name, val); }
    void write(const char* name, const std::string& val) { write_string(name, val); }
    void write(const char* name, long val) { write_integer(name, val); }
    void write(const char* name, unsigned long val) { write_integer(name, val); }
    void write(const char* name, int val) { write_integer(name, val); }
    void write(const char* name, unsigned int val) { write_integer(name, val); }
    void write(const char* name, long long val) { write_integer(name, val); }
    void write(const char* name, unsigned long long val) { write_integer(name, val); }
    void write(const char* name, float val) { write_float(name, val); }
    void write(const char* name, double val) { write_float(name, val); }
    void write(const char* name, long double val) { write_float(name, val); }
};


structured_writer* create_xml_writer(output_stream* os, bool compressed);
structured_writer* create_json_writer(output_stream* os, bool compressed);


}
