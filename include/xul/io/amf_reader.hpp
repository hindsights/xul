#pragma once

/**
 * @file
 * @brief amf(binary message format)
 */

#include <xul/io/amf_types.hpp>
#include <xul/io/data_input_stream.hpp>
#include <xul/std/maps.hpp>
#include <xul/data/variant.hpp>
#include <xul/data/list.hpp>
#include <xul/data/map.hpp>
#include <boost/noncopyable.hpp>
#include <stddef.h>
#include <stdint.h>


namespace xul {


/// amf(binary message format)
class amf0_reader : boost::noncopyable, public amf0_types
{
public:
    class null_type
    {
    public:
        null_type() { }
    };

    explicit amf0_reader(data_input_stream& is) : m_is(is)
    {
    }
    virtual ~amf0_reader()
    {
    }

    variant* read()
    {
        if (false == m_is.is_available(1))
        {
            return NULL;
        }
        variant* var = create_variant();
        uint8_t tag = m_is.read_byte();
        return read_item(tag, var) ? var : NULL;
    }

    list<variant*>* read_many()
    {
        list<variant*>* vars = create_variant_list();
        for (;;)
        {
            boost::intrusive_ptr<variant> var = read();
            if (!var)
            {
                XUL_APP_DEBUG("read failed");
                break;
            }
            vars->add(var.get());
        }
        return vars;
    }


private:
    bool read_item(int tag, variant* var)
    {
        switch (tag)
        {
        case amf0_null:
        case amf0_undefined:
            var->set_null();
            return true;
        case amf0_boolean:
            return read_bool(var);
        case amf0_number:
            return read_number(var);
        case amf0_string:
            return read_string(var);
        case amf0_array:
            return read_list(var);
        case amf0_object:
            return read_dict(var);
        }
        return false;
    }
    bool read_bool(variant* var)
    {
        uint8_t val = m_is.read_byte(val);
        var->set_bool(val != 0);
        return true;
    }
    bool read_number(variant* var)
    {
        double val = 0;
        if (!m_is.read_double(val))
            return false;
        var->set_float(val);
        return true;
    }
    bool read_string(variant* var)
    {
        std::string s;
        if (false == try_read_string(s))
        {
            var->set_null();
            return false;
        }
        var->set_bytes(s.c_str(), s.size());
        return true;
    }
    bool try_read_string(std::string& s)
    {
        uint16_t len = 0;
        if (false == m_is.read_uint16(len))
            return false;
        s.resize(static_cast<size_t>(len));
        if (false == m_is.read_n(reinterpret_cast<uint8_t*>(&s[0]), static_cast<size_t>(len)))
            return false;
        return true;
    }
    bool read_list(variant* var)
    {
        variant::list_type* items = var->set_list();
        for (;;)
        {
            if (false == m_is.is_available(1))
                return false;
            uint8_t tag = m_is.read_byte();
            boost::intrusive_ptr<variant> item = create_variant();
            if (read_item(tag, item.get()))
            {
                items->add(item.get());
            }
        }
        return false;
    }
    bool read_dict(variant* var)
    {
        variant::dict_type* items = var->set_dict();
        for (;;)
        {
            if (false == m_is.is_available(1))
                return true;
            std::string key;
            if (false == try_read_string(key))
                return true;
            boost::intrusive_ptr<variant> item = read();
            if (item)
            {
                items->set(key.c_str(), item.get());
            }
        }
        return true;
    }

private:
    data_input_stream& m_is;
};


}
