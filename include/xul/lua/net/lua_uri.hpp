#pragma once

#include <xul/net/uri.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_uri
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "new", &lua_uri::create },
            { "parse", &lua_uri::parse },
            { "get_host", &lua_uri::get_host },
            { "get_port", &lua_uri::get_port },
            { "get_path", &lua_uri::get_path },
            { "get_param", &lua_uri::get_param },
            { "get_params", &lua_uri::get_params },
            { "get_original_string", &lua_uri::get_original_string },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.uri", get_methods());
    }

    static int create(lua_State* L)
    {
        uri* obj = create_uri();
        lua_utility::register_object(L, obj, "xul.uri");
        return 1;
    }
    static int get_path(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -1);
        if (!obj)
            return -1;
        const char* s = obj->get_path();
        lua_pushstring(L, s);
        return 1;
    }
    static int get_original_string(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -1);
        if (!obj)
            return -1;
        const char* s = obj->get_original_string();
        lua_pushstring(L, s);
        return 1;
    }
    static int get_host(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -1);
        if (!obj)
            return -1;
        const char* s = obj->get_host();
        lua_pushstring(L, s);
        return 1;
    }
    static int get_port(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -1);
        if (!obj)
            return -1;
        int port = obj->get_port();
        lua_pushnumber(L, port);
        return 1;
    }
    static int get_param(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -2);
        if (!obj)
            return -1;
        std::string key;
        if (!lua_utility::try_get_string(L, -1, &key))
            return -1;
        const char* val = obj->get_param(key.c_str());
        lua_pushstring(L, val);
        return 1;
    }
    static int get_params(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -1);
        if (!obj)
            return -1;
        string_table* params = const_cast<string_table*>(obj->get_params());
        lua_pushlightuserdata(L, params);
        return 1;
    }
    static int parse(lua_State* L)
    {
        uri* obj = lua_utility::get_object<uri>(L, -2);
        if (!obj)
            return -1;
        const char* s = lua_tostring(L, -1);
        if (!s)
            return -1;
        bool ret = obj->parse(s);
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
};


}
