#pragma once

#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_string
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_functions[] =
        {
            { "read_memory", &read_memory },
            { NULL, NULL },
        };
        return the_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.string", get_methods());
    }

    static int read_memory(lua_State* L)
    {
        if (lua_type(L, 1) != LUA_TLIGHTUSERDATA)
            return -1;
        const char* s = static_cast<const char*>(lua_touserdata(L, 1));
        int size = lua_utility::get_integer(L, 2, -1);
        if (size < 0 || NULL == s)
        {
            return -1;
        }
        if (0 == size)
        {
            lua_pushlstring(L, "", 0);
            return 1;
        }
        int64_t startpos, endpos;
        if (!lua_utility::try_get_integer(L, 3, &startpos))
        {
            startpos = 0;
        }
        if (!lua_utility::try_get_integer(L, 4, &endpos))
        {
            endpos = size;
        }
        XUL_APP_DEBUG("read_memory " << xul::make_tuple(static_cast<const void*>(s), size, startpos, endpos));
        if (startpos < 0)
            startpos = size + startpos;
        if (startpos > size)
            startpos = size;
        if (endpos < 0)
            endpos = size + endpos;
        if (endpos > size)
            endpos = size;
        if (endpos < startpos)
            endpos = startpos;
        lua_pushlstring(L, s + startpos, endpos - startpos);
        return 1;
    }
};


}
