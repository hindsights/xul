#pragma once

#include <xul/net/url_session.hpp>
#include <xul/net/url_sessions.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/lua.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_url_session
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "create_wrapper", &lua_url_session::create_wrapper },
            { "close", &lua_url_session::close },
            { "finish", &lua_url_session::finish },
            { "get_connection", &lua_url_session::get_connection },
            { "get_response", &lua_url_session::get_response },
            { "send_header", &lua_url_session::send_header },
            { "send_data", &lua_url_session::send_data },
            { "is_chunked", &lua_url_session::is_chunked },
            { "set_chunked", &lua_url_session::set_chunked },
            { "send_empty_response", &lua_url_session::send_empty_response },
            { "send_simple_response", &lua_url_session::send_simple_response },
            { "send_plain_text", &lua_url_session::send_plain_text },
            { "redirect_permanently", &lua_url_session::redirect_permanently },
            { "redirect_temporarily", &lua_url_session::redirect_temporarily },
            { NULL, NULL },
        };
        return the_methods;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.url_session", get_methods());
    }

    static url_session* get_self(lua_State* L)
    {
        url_session* self = lua_utility::get_object<url_session>(L, 1);
        assert(self);
        return self;
    }

    static int create_wrapper(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        lua_utility::register_object(L, self, "xul.url_session");
        return 1;
    }
    static int close(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        self->close();
        return 0;
    }

    static int finish(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        self->finish();
        return 0;
    }
    static int get_connection(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        tcp_socket* conn = self->get_connection();
        lua_utility::register_object(L, conn, "xul.tcp_socket");
        return 1;
    }
    static int get_response(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        url_response* resp = self->get_response();
        lua_utility::register_object(L, resp, "xul.url_response");
        return 1;
    }

    static int send_header(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        bool ret = self->send_header();
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }

    static int send_data(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        int datatype = lua_type(L, 2);
        if (LUA_TSTRING == datatype)
        {
            size_t len = 0;
            const char* data = lua_tolstring(L, 2, &len);
            if (!data || len <= 0)
                return -1;
            bool ret = self->send_data((const uint8_t*)data, (int)len);
            lua_pushboolean(L, ret ? 1 : 0);
        }
        else if (LUA_TLIGHTUSERDATA == datatype)
        {
            const uint8_t* data = static_cast<const uint8_t*>(lua_touserdata(L, 2));
            int len = lua_tonumber(L, 3);
            if (!data || len <= 0)
                return -1;
            bool ret = self->send_data(data, len);
            lua_pushboolean(L, ret ? 1 : 0);
        }
        else
        {
            assert(false);
            return -1;
        }
        return 1;
    }

    static int is_chunked(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        bool chunked = self->is_chunked();
        lua_pushboolean(L, chunked ? 1 : 0);
        return 1;
    }
    static int set_chunked(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        int chunked = lua_toboolean(L, 2);
        if (chunked != 1 && chunked != 0)
            return -1;
        self->set_chunked(1 == chunked);
        return 0;
    }

    static int send_empty_response(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        int status_code = lua_tointeger(L, -1);
        bool ret = url_sessions::send_empty_response(self, status_code);
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
    static int send_simple_response(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        int status_code = lua_tointeger(L, 2);
        size_t len1 = 0, len2 = 0;
        const char* s1 = lua_tolstring(L, 3, &len1);
        const char* s2 = lua_tolstring(L, 4, &len2);
        if (!s1 || !s2)
            return -1;
        std::string content(s1, len1);
        std::string content_type(s2, len2);
        bool ret = url_sessions::send_simple_response(self, status_code, content.c_str(), content_type.c_str());
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
    static int send_plain_text(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        int status_code = lua_tointeger(L, 2);
        size_t len1 = 0, len2 = 0;
        const char* s1 = lua_tolstring(L, 3, &len1);
        if (!s1)
            return -1;
        std::string content(s1, len1);
        bool ret = url_sessions::send_plain_text(self, status_code, content.c_str());
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
    static int redirect_permanently(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        size_t len = 0;
        const char* s = lua_tolstring(L, 1, &len);
        if (!s)
            return -1;
        std::string urlstr(s, len);
        bool ret = url_sessions::redirect_permanently(self, urlstr.c_str());
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
    static int redirect_temporarily(lua_State* L)
    {
        url_session* self = get_self(L);
        if (!self)
            return -1;
        size_t len = 0;
        const char* s = lua_tolstring(L, 2, &len);
        if (!s)
            return -1;
        std::string urlstr(s, len);
        bool ret = url_sessions::redirect_temporarily(self, urlstr.c_str());
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
};


}
