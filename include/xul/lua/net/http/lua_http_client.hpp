#pragma once

#include <xul/net/http/http_client.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_callback.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>


namespace xul {


class lua_http_client_callback
{
public:
    explicit lua_http_client_callback(lua_State* L) : m_callback(L)
    {
    }

    void download_data_callback(
        http_client* client,
        boost::intrusive_ptr<uri> url, int errcode, const url_response* resp,
        const uint8_t* data, size_t size)
    {
        lua_State* L = m_callback.get_lua_state();
        m_callback.prepare_function_invoke();
        if (!lua_isfunction(L, -1))
        {
            XUL_APP_ERROR("download_data_callback no callback");
            return;
        }
        lua_utility::register_object(L, url.get(), "xul.uri");
        lua_pushnumber(L, errcode);
        lua_utility::register_object(L, const_cast<url_response*>(resp), "xul.url_response");
        lua_pushlstring(L, reinterpret_cast<const char*>(data), size);
        lua_utility::pcall(L, 4);
    }
private:
    lua_callback m_callback;
};


class lua_http_client
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_http_client::create },
            { "download_data", &lua_http_client::download_data },
            { "post_data", &lua_http_client::post_data },
            { "close", &lua_http_client::close },
            { "is_started", &lua_http_client::is_started },
            { NULL, NULL },
        };
        return the_methods;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.http_client", get_methods());
    }

    static int create(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        http_client* client = new http_client(ios);
        lua_utility::register_object(L, client, "xul.http_client");
        return 1;
    }
    static int close(lua_State* L)
    {
        http_client* client = lua_utility::get_object<http_client>(L, -1);
        if (!client)
            return -1;
        client->close();
        return 0;
    }
    static int is_started(lua_State* L)
    {
        http_client* client = lua_utility::get_object<http_client>(L, -1);
        if (!client)
            return -1;
        xul::lua_utility::push_bool(L, client->is_started());
        return 0;
    }
    static int download_data(lua_State* L)
    {
        http_client* client = lua_utility::get_object<http_client>(L, 1);
        if (!client)
            return -1;
        std::string urlstr = lua_utility::get_string(L, 2);
        int maxsize = lua_utility::get_integer(L, 3, 1024*1024);
        boost::shared_ptr<lua_http_client_callback> callback(new lua_http_client_callback(L));
        client->async_download_data(urlstr, maxsize, boost::bind(&lua_http_client_callback::download_data_callback, callback, _1, _2, _3, _4, _5, _6));
        return 0;
    }
    static int post_data(lua_State* L)
    {
        http_client* client = lua_utility::get_object<http_client>(L, -5);
        if (!client)
            return -1;
        std::string urlstr = lua_utility::get_string(L, -4);
        boost::intrusive_ptr<uri> uri = create_uri();
        if (!uri->parse(urlstr.c_str()))
            return -1;
        std::string body, contentType;
        if (!lua_utility::try_get_string(L, -3, &body) || !lua_utility::try_get_string(L, -2, &contentType))
            return -1;
        boost::shared_ptr<lua_http_client_callback> callback(new lua_http_client_callback(L));
        client->async_post_data(uri.get(), 4096,
            boost::bind(&lua_http_client_callback::download_data_callback, callback, _1, _2, _3, _4, _5, _6),
            body, contentType, NULL);
        return 0;
    }
};


}
