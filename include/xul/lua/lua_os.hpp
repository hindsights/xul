#pragma once

#include <xul/os/paths.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_paths
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg paths_functions[] =
        {
            { "join", &join },
            { NULL, NULL },
        };
        return paths_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.paths", get_methods());
    }

    static int join(lua_State* L)
    {
        std::string dir = lua_utility::get_string(L, 1);
        std::string name = lua_utility::get_string(L, 2);
        std::string path = paths::join(dir, name);
        lua_pushlstring(L, path.c_str(), path.size());
        return 1;
    }
};


}
