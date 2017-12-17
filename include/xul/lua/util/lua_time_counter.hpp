#pragma once

#include <xul/util/time_counter.hpp>
#include <xul/posix/gettimeofday.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_time_counter
{
public:
    static void register_functions(lua_State* L)
    {
        lua_utility::register_function(L, "xul", "get_time_count", &get_time_count);
        lua_utility::register_function(L, "xul", "gettimeofday", &lua_gettimeofday);
    }

    static int get_time_count(lua_State* L)
    {
        int64_t count = time_counter::get_system_count();
        lua_pushnumber(L, count);
        return 1;
    }
    static int lua_gettimeofday(lua_State* L)
    {
        timeval tp;
        gettimeofday(&tp, NULL);
        lua_pushinteger(L, tp.tv_sec);
        lua_pushinteger(L, tp.tv_usec);
        return 2;
    }
};


}
