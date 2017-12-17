#pragma once

#include <xul/data/block_pool.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_block_pool
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "get_block_size", &get_block_size },
            { "get_used_count", &get_used_count },
            { "get_max_count", &get_max_count },
            { "allocate", &allocate },
            { "free", &free },
            { "get_block_size", &get_block_size },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.block_pool", get_methods());
    }

    static int get_block_size(lua_State* L)
    {
        block_pool* pool = lua_utility::get_object<block_pool>(L, 1);
        if (!pool)
        {
            assert(false);
            return -1;
        }
        lua_pushnumber(L, pool->get_block_size());
        return 1;
    }
    static int get_used_count(lua_State* L)
    {
        block_pool* pool = lua_utility::get_object<block_pool>(L, 1);
        if (!pool)
        {
            assert(false);
            return -1;
        }
        lua_pushnumber(L, pool->get_used_count());
        return 1;
    }
    static int get_max_count(lua_State* L)
    {
        block_pool* pool = lua_utility::get_object<block_pool>(L, 1);
        if (!pool)
        {
            assert(false);
            return -1;
        }
        lua_pushnumber(L, pool->get_max_count());
        return 1;
    }
    static int allocate(lua_State* L)
    {
        block_pool* pool = lua_utility::get_object<block_pool>(L, 1);
        if (!pool)
        {
            assert(false);
            return -1;
        }
        lua_utility::register_object(L, pool, "xul.block_pool");
        return 1;
    }
    static int free(lua_State* L)
    {
        block_pool* pool = lua_utility::get_object<block_pool>(L, 1);
        if (!pool)
        {
            assert(false);
            return -1;
        }
        if (lua_type(L, 2) != LUA_TLIGHTUSERDATA)
        {
            assert(false);
            return -1;
        }
        void* buf = lua_touserdata(L, 2);
        pool->deallocate(static_cast<char*>(buf));
        return 0;
    }
};


}
