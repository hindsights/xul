#pragma once

#include <xul/util/timer.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/lua_callback.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_timer_listener : public timer_listener
{
public:
    explicit lua_timer_listener(lua_State* L) : m_callback(L)
    {
        XUL_LOGGER_INIT("lua_timer_listener");
        XUL_DEBUG("new");
    }
    ~lua_timer_listener()
    {
        XUL_DEBUG("delete");
    }

    virtual void on_timer_elapsed(timer* sender)
    {
        lua_State* L = m_callback.get_lua_state();
        lua_utility::dump_stack(L, "on_timer_elapsed");
        m_callback.prepare_function_invoke();
        assert(lua_isfunction(L, -1));
        lua_utility::pcall(L, 0);
        lua_utility::dump_stack(L, "on_timer_elapsed.end");
        //lua_pop(L, 1);
    }

private:
    XUL_LOGGER_DEFINE();
    lua_callback m_callback;
};

class lua_timer
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "is_started", &lua_timer::is_started },
            { "start", &lua_timer::start },
            { "stop", &lua_timer::stop },
            { "get_times", &lua_timer::get_times },
            { "set_listener", &lua_timer::set_listener },
            { "set_callback", &lua_timer::set_listener },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.timer", get_methods());
    }

    static int stop(lua_State* L)
    {
        timer* t = lua_utility::get_object<timer>(L, 1);
        if (!t)
            return -1;
        t->stop();
        return 0;
    }
    static int is_started(lua_State* L)
    {
        timer* t = lua_utility::get_object<timer>(L, 1);
        if (!t)
            return -1;
        bool ret = t->is_started();
        lua_utility::push_bool(L, ret);
        return 1;
    }
    static int start(lua_State* L)
    {
        timer* t = lua_utility::get_object<timer>(L, 1);
        if (!t)
            return -1;
        int interval = lua_tointeger(L, 2);
        t->start(interval);
        return 0;
    }
    static int set_listener(lua_State* L)
    {
        timer* t = xul::lua_utility::get_object<timer>(L, 1);
        if (!t)
            return -1;
        lua_utility::dump_stack(L, "timer.set_listener");
        assert(lua_isfunction(L, 2));
        /// TO-DO: memory leak
        lua_timer_listener* callback = new lua_timer_listener(L);
        t->set_listener(callback);
        return 0;
    }
    static int get_times(lua_State* L)
    {
        timer* t = xul::lua_utility::get_object<timer>(L, 1);
        if (!t)
            return -1;
        lua_pushinteger(L, t->get_times());
        return 1;
    }

};


}
