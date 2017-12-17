#pragma once

#include <xul/net/url_message.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/lua.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_url_message
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_message_functions[] =
        {
            { "clear", &lua_url_message::clear },
            { "get_protocol_name", &lua_url_message::get_protocol_name },
            { "set_protocol_name", &lua_url_message::set_protocol_name },
            { "get_protocol_version", &lua_url_message::get_protocol_version },
            { "set_protocol_version", &lua_url_message::set_protocol_version },
            { "get_headers", &lua_url_message::get_headers },
            { "get_header", &lua_url_message::get_header },
            { "set_header", &lua_url_message::set_header },
            // { "get_content_range", &lua_url_message::get_content_range },
            // { "set_content_range", &lua_url_message::set_content_range },
            // { "get_range", &lua_url_message::get_range },
            // { "set_range", &lua_url_message::set_range },
            { NULL, NULL },
        };
        return url_message_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.url_message", get_methods());
    }

    static url_message* get_self(lua_State* L)
    {
        url_message* msg = lua_utility::get_object<url_message>(L, 1);
        assert(msg);
        return msg;
    }

    static int clear(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        msg->clear();
        return 0;
    }

    static int get_protocol_name(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        const char* s = msg->get_protocol_name();
        lua_pushstring(L, s);
        return 1;
    }
    static int set_protocol_name(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        msg->set_protocol_name(s);
        return 0;
    }

    static int get_protocol_version(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        const char* s = msg->get_protocol_version();
        lua_pushstring(L, s);
        return 1;
    }
    static int set_protocol_version(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        msg->set_protocol_version(s);
        return 0;
    }

    static int get_headers(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        xul::string_table* t = msg->ref_headers();
        lua_pushlightuserdata(L, t);
        return 1;
    }

    static int get_header(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        const char* key = lua_tostring(L, 2);
        if (!key)
            return -1;
        const char* default_val = lua_tostring(L, 3);
        const char* s = msg->get_header(key, default_val ? default_val : "");
        lua_pushstring(L, s);
        return 1;
    }

    static int set_header(lua_State* L)
    {
        url_message* msg = get_self(L);
        if (!msg)
            return -1;
        const char* key = lua_tostring(L, 2);
        if (!key)
            return -1;
        const char* val = lua_tostring(L, 3);
        if (!val)
            return -1;
        msg->set_header(key, val);
        return 0;
    }
};


}
