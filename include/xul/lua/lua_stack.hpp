#pragma once

#include <xul/lua/lua.hpp>
#include <boost/noncopyable.hpp>


namespace xul {


class lua_stack_cleaner : private boost::noncopyable
{
public:
    explicit lua_stack_cleaner(lua_State* L) : m_L(L)
    {
        m_top = lua_gettop(m_L);
    }
    ~lua_stack_cleaner()
    {
    }
    void restore()
    {
        lua_settop(m_L, m_top);
    }

private:
    lua_State* m_L;
    int m_top;
};


class lua_auto_stack_cleaner : private boost::noncopyable
{
public:
    explicit lua_auto_stack_cleaner(lua_State* L) : m_L(L)
    {
        m_top = lua_gettop(m_L);
    }
    ~lua_auto_stack_cleaner()
    {
        lua_settop(m_L, m_top);
    }

private:
    lua_State* m_L;
    int m_top;
};


}
