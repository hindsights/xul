#pragma once

#include <xul/data/buffer.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_buffer
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_buffer::create },
            { "total_count", &lua_buffer::total_count },
            { "reserve", &lua_buffer::reserve },
            { "resize", &lua_buffer::resize },
            { "ensure_size", &lua_buffer::ensure_size },
            { "assign", &lua_buffer::assign },
            { "append", &lua_buffer::append },
            { "clear", &lua_buffer::clear },
            { "size", &lua_buffer::size },
            { "capacity", &lua_buffer::capacity },
            { "empty", &lua_buffer::empty },
            { "data", &lua_buffer::data },
            { "str", &lua_buffer::str },
            { "sub", &lua_buffer::sub },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.buffer", get_methods());
    }

    static int create(lua_State* L)
    {
        byte_buffer* buf = new byte_buffer;
        lua_utility::register_object(L, buf, "xul.buffer");
        return 1;
    }
    static int total_count(lua_State* L)
    {
#if defined(_DEBUG) || !defined(NDEBUG)
        lua_pushnumber(L, byte_buffer::get_total_buffer_count());
#else
        lua_pushnumber(L, 0);
#endif
        return 1;
    }
    static byte_buffer* get_object(lua_State* L, int idx = 1)
    {
        byte_buffer* buf = lua_utility::get_object<byte_buffer>(L, 1);
        assert(buf);
        return buf;
    }
    static int reserve(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        int size = -1;
        if (!lua_utility::try_get_integer(L, 2, &size))
        {
            assert(false);
            return -1;
        }
        buf->reserve(size);
        return 0;
    }
    static int resize(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        int size = -1;
        if (!lua_utility::try_get_integer(L, 2, &size))
        {
            assert(false);
            return -1;
        }
        buf->resize(size);
        return 0;
    }
    static int ensure_size(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        int size = -1;
        if (!lua_utility::try_get_integer(L, 2, &size))
        {
            assert(false);
            return -1;
        }
        buf->ensure_size(size);
        return 0;
    }
    static int assign(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        int datatype = lua_type(L, 2);
        if (LUA_TSTRING == datatype)
        {
            size_t len = 0;
            const char* str = lua_tolstring(L, 2, &len);
            if (str)
            {
                buf->assign(reinterpret_cast<const uint8_t*>(str), len);
                return 0;
            }
            else
            {
                return -1;
            }
        }
        const uint8_t* data = static_cast<const uint8_t*>(lua_utility::get_userdata(L, 2, NULL));
        int size = lua_utility::get_integer(L, 3, -1);
        if (!data || size <0)
        {
            assert(false);
            return -1;
        }
        buf->assign(data, size);
        return 0;
    }
    static int append(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        int datatype = lua_type(L, 2);
        if (LUA_TSTRING == datatype)
        {
            size_t len = 0;
            const char* str = lua_tolstring(L, 2, &len);
            if (str)
            {
                buf->append(reinterpret_cast<const uint8_t*>(str), len);
                return 0;
            }
            else
            {
                return -1;
            }
        }
        const uint8_t* data = static_cast<const uint8_t*>(lua_utility::get_userdata(L, 2, NULL));
        int size = lua_utility::get_integer(L, 3, -1);
        if (!data || size <0)
        {
            assert(false);
            return -1;
        }
        buf->append(data, size);
        return 0;
    }
    static int clear(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        buf->clear();
        return 0;
    }
    static int size(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        lua_pushnumber(L, buf->size());
        return 1;
    }
    static int capacity(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        lua_pushnumber(L, buf->capacity());
        return 1;
    }
    static int empty(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        lua_utility::push_bool(L, buf->empty());
        return 1;
    }
    static int data(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        lua_pushlightuserdata(L, buf->data());
        return 1;
    }
    static int str(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        lua_pushlstring(L, reinterpret_cast<const char*>(buf->data()), buf->size());
        return 1;
    }
    static int sub(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        if (!buf)
            return -1;
        int startpos = lua_utility::get_integer(L, 2, 0);
        int endpos = lua_utility::get_integer(L, 3, (int)buf->size());
        if (startpos < 0)
            startpos += buf->size();
        if (endpos < 0)
            endpos += buf->size();
        if (startpos < 0 || startpos > buf->size() || endpos < 0 || endpos > buf->size() || endpos < startpos)
        {
            assert(false);
            return -1;
        }
        byte_buffer* newbuf = new byte_buffer;
        newbuf->assign(buf->data() + startpos, endpos - startpos);
        lua_utility::register_object(L, newbuf, "xul.buffer");
        return 1;
    }
    static int swap(lua_State* L)
    {
        byte_buffer* buf = get_object(L);
        byte_buffer* buf2 = get_object(L, 2);
        if (!buf || !buf2)
            return -1;
        buf->swap(*buf2);
        return 0;
    }
};


}
