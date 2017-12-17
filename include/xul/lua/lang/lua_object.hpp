#pragma once

#include <xul/lang/object.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


#define XUL_OBJECT "xul.object"


class lua_object
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "get_reference_count", &lua_object::get_reference_count },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, XUL_OBJECT, get_methods());
    }

    static int get_reference_count(lua_State* L)
    {
        object* obj = lua_utility::get_object<object>(L, -1);
        if (!obj)
            return -1;
        lua_pushinteger(L, obj->get_reference_count());
        return 1;
    }
};


}
