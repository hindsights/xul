#pragma once

/**
 * @file
 * @brief amf(binary message format)
 */

#include <xul/io/amf_types.hpp>
#include <xul/io/data_input_stream.hpp>
#include <xul/std/maps.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_utility.hpp>
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include <vector>
#include <list>
#include <map>
#include <stddef.h>
#include <stdint.h>


namespace xul {


/// amf(binary message format)
class lua_amf0_reader : boost::noncopyable, public amf0_types
{
public:
    class null_type
    {
    public:
        null_type() { }
    };

    explicit lua_amf0_reader(data_input_stream& is, lua_State* lstate) : m_is(is), m_lstate(lstate)
    {
    }
    virtual ~lua_amf0_reader()
    {
    }

    bool read_many()
    {
        lua_newtable(m_lstate);
        int top = lua_gettop(m_lstate);
        int i = 1;
        for (;;)
        {
            lua_pushnumber(m_lstate, i);
            if (!read())
            {
                XUL_APP_DEBUG("read failed");
                lua_utility::dump_stack(m_lstate, "read_many1");
                lua_pop(m_lstate, 1);
                lua_utility::dump_stack(m_lstate, "read_many2");
                break;
            }
            lua_utility::dump_stack(m_lstate, "read_many3");
            lua_settable(m_lstate, -3);
            lua_utility::dump_stack(m_lstate, "read_many4");
            ++i;
        }
        lua_utility::dump_stack(m_lstate, "read_many5");
        //lua_pushvalue(m_lstate, top);
        //lua_utility::dump_stack(m_lstate);
        return true;
    }

    bool read()
    {
        if (false == m_is.is_available(1))
            return false;
        uint8_t tag = m_is.read_byte();
        return read_item(tag);
    }

private:
    bool read_item(int tag)
    {
        switch (tag)
        {
        case amf0_null:
        case amf0_undefined:
            lua_pushnil(m_lstate);
            return true;
        case amf0_boolean:
            return read_bool();
        case amf0_number:
            return read_number();
        case amf0_string:
            return read_string();
        case amf0_array:
            return read_list();
        case amf0_object:
            return read_dict();
        case 0xFF:
            return false;
        }
        return false;
    }
    bool read_bool()
    {
        uint8_t val = m_is.read_byte(val);
        if (!m_is)
            return false;
        lua_pushboolean(m_lstate, val != 0 ? 1 : 0);
        return true;
    }
    bool read_number()
    {
        double val = 0;
        if (!m_is.read_double(val))
            return false;
        lua_pushnumber(m_lstate, val);
        return true;
    }
    bool read_string()
    {
        std::string s;
        if (false == try_read_string(s))
            return false;
        lua_pushlstring(m_lstate, s.c_str(), s.size());
        return true;
    }
    bool try_read_string(std::string& s)
    {
        uint16_t len = 0;
        if (false == m_is.read_uint16(len))
            return false;
        if (len > 128 * 1024)
            return false;
        s.resize(static_cast<size_t>(len));
        if (false == m_is.read_n(reinterpret_cast<uint8_t*>(&s[0]), static_cast<size_t>(len)))
            return false;
        //assert(!s.empty());
        return true;
    }
    bool read_list()
    {
        lua_newtable(m_lstate);
        int i = 1;
        for (;;)
        {
            if (false == m_is.is_available(1))
            {
                XUL_APP_DEBUG("read_list failed to read key");
                lua_utility::dump_stack(m_lstate, "read_list1");
                return false;
            }
            uint8_t tag = m_is.read_byte();
            lua_pushnumber(m_lstate, i);
            if (!read_item(tag))
            {
                lua_utility::dump_stack(m_lstate, "read_list2");
                lua_pop(m_lstate, 1);
                lua_utility::dump_stack(m_lstate, "read_list3");
                return false;
            }
            lua_utility::dump_stack(m_lstate, "read_list4");
            lua_settable(m_lstate, -3);
            lua_utility::dump_stack(m_lstate, "read_list5");
            ++i;
        }
        lua_utility::dump_stack(m_lstate, "read_list6");
        return true;
    }
    bool read_dict()
    {
        lua_newtable(m_lstate);
        for (;;)
        {
            if (false == m_is.is_available(1))
            {
                XUL_APP_DEBUG("read_dict no data");
                lua_utility::dump_stack(m_lstate, "read_dict1");
                return false;
            }
            std::string key;
            if (false == try_read_string(key))
            {
                XUL_APP_DEBUG("read_dict failed to read key");
                lua_utility::dump_stack(m_lstate, "read_dict2");
                return false;
            }
            if (key.empty() && m_is.read_byte() == 0x09)
            {
                XUL_APP_DEBUG("read_dict end");
                lua_utility::dump_stack(m_lstate, "read_dict3");
                break;
            }
            lua_pushlstring(m_lstate, key.c_str(), key.size());
            if (!read())
            {
                lua_utility::dump_stack(m_lstate, "read_dict4");
                lua_pop(m_lstate, 1);
                lua_utility::dump_stack(m_lstate, "read_dict5");
                XUL_APP_DEBUG("read_dict failed to read val " << key);
                return false;
            }
            lua_utility::dump_stack(m_lstate, "read_dict6");
            lua_settable(m_lstate, -3);
            lua_utility::dump_stack(m_lstate, "read_dict7");
        }
        lua_utility::dump_stack(m_lstate, "read_dict8");
        return true;
    }

private:
    data_input_stream& m_is;
    lua_State* m_lstate;
};


}
