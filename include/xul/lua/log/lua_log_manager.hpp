#pragma once

#include <xul/log/log.hpp>
#include <xul/log/log_manager.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_log_manager
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "configure_filter_chain", &lua_log_manager::configure_filter_chain },
            { "start_console_log_service", &lua_log_manager::start_console_log_service },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.log_manager", get_methods());
    }

    static int configure_filter_chain(lua_State* L)
    {
        options* opts = lua_utility::get_object<options>(L, 1);
        bool ret = log_manager::configure_filter_chain(opts);
        lua_utility::push_bool(L, ret);
        return 1;
    }
    static int start_console_log_service(lua_State* L)
    {
        std::string name = lua_utility::get_string(L, 1);
        int level = lua_utility::get_integer(L, 2, LOG_DEBUG);
        bool ret = log_manager::start_console_log_service(name, level);
        lua_utility::push_bool(L, ret);
        return 1;
    }

};


}
