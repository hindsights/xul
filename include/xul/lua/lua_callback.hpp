#pragma once

#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>
#include <boost/noncopyable.hpp>


namespace xul {


class lua_callback : private boost::noncopyable
{
public:
    explicit lua_callback(lua_State* L) : m_lstate(L)
    {
        XUL_LOGGER_INIT("lua_callback");
        XUL_DEBUG("new");
        lua_utility::dump_stack(L, "lua_callback");
        m_callback_ref = luaL_ref(m_lstate, LUA_REGISTRYINDEX);
        lua_utility::dump_stack(L, "lua_callback1");
    }
    ~lua_callback()
    {
        XUL_DEBUG("delete");
        luaL_unref(m_lstate, LUA_REGISTRYINDEX, m_callback_ref);
    }

    lua_State* get_lua_state()
    {
        return m_lstate;
    }
    int get_callback_ref()
    {
        return m_callback_ref;
    }

    bool prepare_table_invoke(const char* name)
    {
        XUL_DEBUG("prepare_table_invoke " << name);
        lua_State* L = get_lua_state();
        // prepare callback function
        lua_rawgeti(L, LUA_REGISTRYINDEX, get_callback_ref());
        xul::lua_utility::dump_stack(L, "prepare_table_invoke");
        if (!lua_istable(L, -1))
            return false;
        lua_pushstring(L, name);
        lua_gettable(L, -2);
        xul::lua_utility::dump_stack(L, "prepare_table_invoke");
        assert(lua_isfunction(L, -1) || lua_isnil(L, -1));
        return lua_isfunction(L, -1);
    }

    bool prepare_object_invoke(const char* name)
    {
        XUL_DEBUG("prepare_object_invoke " << name);
        lua_State* L = get_lua_state();
        // prepare callback function
        xul::lua_utility::dump_stack(L, "prepare_object_invoke0");
        lua_rawgeti(L, LUA_REGISTRYINDEX, get_callback_ref());
        xul::lua_utility::dump_stack(L, "prepare_object_invoke1");
        // get metatable from object
        if (lua_getmetatable(L, -1) != 1)
        {
            if (!lua_istable(L, -1))
            {
                XUL_REL_WARN("prepare_object_invoke no metatable " << name);
                assert(false);
                return false;
            }
        }
        xul::lua_utility::dump_stack(L, "prepare_object_invoke2");
        assert(lua_istable(L, -1));
        lua_pushstring(L, name);
        lua_gettable(L, -2);
        assert(lua_isfunction(L, -1) || lua_isnil(L, -1));
        bool ret = lua_isfunction(L, -1);
        xul::lua_utility::dump_stack(L, "prepare_object_invoke3");
//        lua_pushvalue(L, -2);
        return ret;
    }

    bool get_object_invoke(const char* name)
    {
        XUL_DEBUG("get_object_invoke " << name);
        lua_State* L = get_lua_state();
        if (!lua_istable(L, -1))
            return false;
        lua_getmetatable(L, -1); // get metatable from object
        if (!lua_istable(L, -1))
            return false;
        lua_pushstring(L, name);
        lua_gettable(L, -2);
        assert(lua_isfunction(L, -1) || lua_isnil(L, -1));
        bool ret = lua_isfunction(L, -1);
        lua_pushvalue(L, -3);
        xul::lua_utility::dump_stack(L, "prepare_object_invoke2");
        return ret;
    }

    void prepare_function_invoke()
    {
        XUL_DEBUG("prepare_function_invoke ");
        lua_State* L = get_lua_state();
        xul::lua_utility::dump_stack(L, "prepare_function_invoke.before");
        // prepare callback function
        lua_rawgeti(L, LUA_REGISTRYINDEX, get_callback_ref());
        xul::lua_utility::dump_stack(L, "prepare_function_invoke");
        assert(lua_isfunction(L, -1));
    }

private:
    XUL_LOGGER_DEFINE();
    lua_State* m_lstate;
    int m_callback_ref;
};


}
