#pragma once

/**
 * @file
 * @brief amf(binary message format)
 */

#include <xul/io/amf_types.hpp>
#include <xul/io/amf_writer.hpp>
#include <xul/io/data_output_stream.hpp>
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



class lua_amf0_writer : public amf0_types
{
public:
    explicit lua_amf0_writer(data_output_stream& os, lua_State* L)
        : m_os(os)
        , m_lstate(L)
        , m_writer(os)
    {
    }
    virtual ~lua_amf0_writer()
    {
    }

    bool write_multiple_at(int idx)
    {
        lua_utility::dump_stack(m_lstate, "write_multiple_at");
        if (!lua_istable(m_lstate, idx))
        {
            assert(false);
            return false;
        }
        //lua_pushnumber(m_lstate, -1);
#if 0
        lua_pushnil(m_lstate);
        while (lua_next(m_lstate, idx - 1) != 0)
        {
            XUL_APP_DEBUG("key " << lua_type(m_lstate, -2) << " " << lua_tonumber(m_lstate, -2));
            XUL_APP_DEBUG("value:" << " " << lua_type(m_lstate, -1));
            do_write_one(-1);
            lua_utility::dump_stack(m_lstate, "after do_write_one");
            lua_pop(m_lstate, 1);
        }
#else
        int item_count = lua_objlen(m_lstate, idx);
        for (int i = 1; i <= item_count; ++i)
        {
            lua_rawgeti(m_lstate, idx, i);
            XUL_APP_DEBUG("item " << i << " " << lua_type(m_lstate, -1));
            lua_utility::dump_stack(m_lstate, "before do_write_one");
            do_write_one(-1);
            lua_pop(m_lstate, 1);
        }
#endif
        return true;
    }

    bool write_multiple(int count)
    {
        for (int idx = 0 - count; idx < 0; ++idx)
        {
            if (!do_write_one(idx))
                return false;
        }
        return true;
    }

    bool do_write_one(int idx)
    {
        int type = lua_type(m_lstate, idx);
        if (type == LUA_TSTRING)
        {
            return write_string(idx);
        }
        else if (type == LUA_TNUMBER)
        {
            return write_number(idx);
        }
        else if (type == LUA_TTABLE)
        {
            lua_utility::dump_stack(m_lstate, "before write_dict");
            XUL_APP_DEBUG("table " << lua_objlen(m_lstate, idx));
            return write_dict(idx);
        }
        else if (type == LUA_TBOOLEAN)
        {
            return write_bool(idx);
        }
        else if (type == LUA_TNIL)
        {
            XUL_APP_DEBUG("write_null " << idx);
            m_writer.write_null();
            return true;
        }
        return false;
    }

    bool write_string(int idx)
    {
        const char* s = luaL_checkstring(m_lstate, idx);
        if (!s)
        {
            assert(false);
            return false;
        }
        XUL_APP_DEBUG("write_string " << idx << " " << s);
        m_writer.write_string(s);
        return true;
    }
    bool write_number(int idx)
    {
        if (!lua_isnumber(m_lstate, idx))
        {
            assert(false);
            return false;
        }
        double val = lua_tonumber(m_lstate, idx);
        XUL_APP_DEBUG("write_number " << idx << " " << val);
        m_writer.write_number(val);
        return true;
    }
    bool write_bool(int idx)
    {
        if (!lua_isboolean(m_lstate, idx))
        {
            assert(false);
            return false;
        }
        int val = lua_toboolean(m_lstate, idx);
        XUL_APP_DEBUG("write_bool " << idx << " " << val);
        m_writer.write_bool(val != 0);
        return true;
    }
    bool write_list(int idx)
    {
        XUL_APP_DEBUG("write_list " << idx);
        assert(false);
        return false;
    }
    bool write_dict(int idx)
    {
        lua_utility::dump_stack(m_lstate, "write_dict 1");
        XUL_APP_DEBUG("write_dict " << idx);
        if (!lua_istable(m_lstate, idx))
        {
            assert(false);
            return false;
        }
        m_writer.start_dict();
        int item_count = lua_objlen(m_lstate, idx);
        XUL_APP_DEBUG("table len " << item_count);
        lua_pushnil(m_lstate);
        while (lua_next(m_lstate, idx - 1) != 0)
        {
            const char* key = luaL_checkstring(m_lstate, -2);
            if (!key)
                return false;
            int value_type = lua_type(m_lstate, -1);
            if (value_type == LUA_TNUMBER)
            {
                double val = lua_tonumber(m_lstate, -1);
                XUL_APP_DEBUG("write_number_element " << key << " " << val);
                m_writer.write_number_element(key, val);
                lua_pop(m_lstate, 1);
            }
            else if (value_type == LUA_TSTRING)
            {
                const char* val = lua_tostring(m_lstate, -1);
                if (val)
                {
                    XUL_APP_DEBUG("write_string_element " << key << " " << val);
                    m_writer.write_string_element(key, val);
                }
                else
                {
                    assert(false);
                }
                lua_pop(m_lstate, 1);
            }
            else if (value_type == LUA_TTABLE)
            {
                XUL_APP_DEBUG("write_dict " << key << " " << idx);
                m_writer.write_key(key);
                write_dict(-1);
                lua_pop(m_lstate, 1);
            }
            else
            {
                lua_pop(m_lstate, 1);
            }
        }
        m_writer.end_dict();
        return true;
    }

private:
    amf0_writer m_writer;
    data_output_stream& m_os;
    lua_State* m_lstate;
};


}
