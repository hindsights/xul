#pragma once

#include <xul/lua/net/lua_url_message.hpp>
#include <xul/net/url_response.hpp>
#include <xul/lua/lua.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_url_response
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_response_functions[] =
        {
#if 1
            { "clear", &lua_url_message::clear },
            { "get_protocol_name", &lua_url_message::get_protocol_name },
            { "set_protocol_name", &lua_url_message::set_protocol_name },
            { "get_protocol_version", &lua_url_message::get_protocol_version },
            { "set_protocol_version", &lua_url_message::set_protocol_version },
            { "get_headers", &lua_url_message::get_headers },
            { "get_header", &lua_url_message::get_header },
            { "set_header", &lua_url_message::set_header },
#endif
            { "get_status_code", &lua_url_response::get_status_code },
            { "set_status_code", &lua_url_response::set_status_code },
            { "get_status_description", &lua_url_response::get_status_description },
            { "set_status_description", &lua_url_response::set_status_description },
            { NULL, NULL },
        };
        return url_response_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.url_response", get_methods());
    }

    static url_response* get_self(lua_State* L)
    {
        url_response* self = lua_utility::get_object<url_response>(L, 1);
        assert(self);
        return self;
    }

    static int get_status_code(lua_State* L)
    {
        xul::url_response* self = get_self(L);
        if (!self)
            return -1;
        int val = self->get_status_code();
        lua_pushinteger(L, val);
        return 1;
    }
    static int set_status_code(lua_State* L)
    {
        xul::url_response* self = get_self(L);
        if (!self)
            return -1;
        int val = lua_tointeger(L, 2);
        self->set_status_code(val);
        return 0;
    }

    static int get_status_description(lua_State* L)
    {
        xul::url_response* self = get_self(L);
        if (!self)
            return -1;
        const char* s = self->get_status_description();
        lua_pushstring(L, s);
        return 1;
    }
    static int set_status_description(lua_State* L)
    {
        xul::url_response* self = get_self(L);
        if (!self)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        self->set_status_description(s);
        return 0;
    }
};


}
