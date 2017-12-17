#pragma once

#include <xul/lua/net/lua_url_message.hpp>
#include <xul/net/url_request.hpp>
#include <xul/lua/lua.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_url_request
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "create_wrapper", &lua_url_request::create_wrapper },
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
            { "get_method", &lua_url_request::get_method },
            { "set_method", &lua_url_request::set_method },
            { "get_url", &lua_url_request::get_url },
            { "set_url", &lua_url_request::set_url },
            { "get_uri", &lua_url_request::get_uri },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.url_request", get_methods());
    }

    static url_request* get_self(lua_State* L)
    {
        url_request* self = lua_utility::get_object<url_request>(L, 1);
        assert(self);
        return self;
    }

    static int create_wrapper(lua_State* L)
    {
        url_request* self = get_self(L);
        if (!self)
            return -1;
        lua_utility::register_object(L, self, "xul.url_request");
        return 1;
    }
    static int get_method(lua_State* L)
    {
        url_request* self = get_self(L);
        if (!self)
            return -1;
        const char* s = self->get_method();
        lua_pushstring(L, s);
        return 1;
    }
    static int set_method(lua_State* L)
    {
        url_request* self = get_self(L);
        if (!self)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        self->set_method(s);
        return 0;
    }

    static int get_uri(lua_State* L)
    {
        url_request* self = get_self(L);
        if (!self)
            return -1;
        const uri* obj = self->get_uri();
        lua_utility::register_object(L, const_cast<uri*>(obj), "xul.uri");
        return 1;
    }
    static int get_url(lua_State* L)
    {
        url_request* self = get_self(L);
        if (!self)
            return -1;
        const char* s = self->get_url();
        lua_pushstring(L, s);
        return 1;
    }
    static int set_url(lua_State* L)
    {
        url_request* self = get_self(L);
        if (!self)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        self->set_url(s);
        return 0;
    }
};


}
