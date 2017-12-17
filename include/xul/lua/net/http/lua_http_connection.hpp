#pragma once

#include <xul/net/http/http_connection.hpp>
#include <xul/net/url_session.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_stack.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_http_connection_handler : public xul::object_impl<http_connection_handler>
{
public:
    explicit lua_http_connection_handler(lua_State* state) : m_callback(state)
    {
        XUL_LOGGER_INIT("lua_http_connection_handler");
        XUL_DEBUG("new");
    }
    ~lua_http_connection_handler()
    {
        XUL_DEBUG("delete");
    }
    virtual bool on_http_response(http_connection* sender, url_response* resp, const char* real_url)
    {
        XUL_DEBUG("on_http_response " << real_url << " " << *resp);
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("on_http_response"))
        {
            XUL_DEBUG("on_http_response ignored " << real_url);
            return false;
        }
        lua_utility::register_object(L, sender, "xul.http_connection");
        lua_utility::register_object(L, const_cast<url_response*>(resp), "xul.url_response");
        lua_pushstring(L, real_url);
        lua_utility::pcall(L, 3);
        xul::lua_utility::dump_stack(L, "on_http_response");
        bool ret = lua_utility::pop_bool(L, true);
        return ret;
    }
    virtual bool on_http_data(http_connection* sender, const uint8_t* data, int size)
    {
        XUL_DEBUG("on_http_data " << size);
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        lua_utility::dump_stack(L, "on_http_data");
        if (m_callback.prepare_object_invoke("on_http_data"))
        {
            lua_utility::register_object(L, sender, "xul.http_connection");
            lua_pushlstring(L, reinterpret_cast<const char*>(data), size);
            lua_utility::pcall(L, 2);
            xul::lua_utility::dump_stack(L, "on_http_data");
            bool ret = lua_utility::pop_bool(L, true);
            return ret;
        }
        return false;
    }
    virtual void on_http_put_content(http_connection* sender)
    {
    }
    virtual void on_http_complete(http_connection* sender, int64_t size)
    {
        XUL_DEBUG("on_http_complete " << size);
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("on_http_complete"))
            return;
        lua_utility::register_object(L, sender, "xul.http_connection");
        lua_pushnumber(L, size);
        lua_utility::pcall(L, 2);
    }
    virtual void on_http_error(http_connection* sender, int errcode)
    {
        XUL_DEBUG("on_http_error " << errcode);
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("on_http_error"))
            return;
        lua_utility::register_object(L, sender, "xul.http_connection");
        lua_pushinteger(L, errcode);
        lua_utility::pcall(L, 2);
    }

private:
    XUL_LOGGER_DEFINE();
    lua_callback m_callback;
};


class lua_http_connection
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_http_connection::create },
            { "download", &lua_http_connection::download },
            { "set_handler", &lua_http_connection::set_handler },
            { "close", &lua_http_connection::close },
            { "pause", &lua_http_connection::pause },
            { "resume", &lua_http_connection::resume },
            { "is_started", &lua_http_connection::is_started },
            { "is_paused", &lua_http_connection::is_paused },
            { "get_options", &lua_http_connection::get_options },
            { "set_receive_buffer_size", &lua_http_connection::set_receive_buffer_size },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.http_connection", get_methods());
        lua_utility::register_function(L, "xul", "create_concurrent_http_connection", &lua_http_connection::create_concurrent);
        //lua_utility::register_function(L, "xul", "create_http_connection_handler", &create_handler);
    }

    static http_connection* get_self(lua_State* L)
    {
        http_connection* self = lua_utility::get_object<http_connection>(L, 1);
        if (self)
            return self;
        luaL_error(L, "xul.http_connection invalid self");
        return NULL;
    }

    // static int create_handler(lua_State* L)
    // {
    //     boost::intrusive_ptr<http_connection_handler> handler = new lua_http_connection_handler(L);
    //     lua_utility::register_object(L, handler.get(), "xul.object");
    //     return 1;
    // }
    static int create(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, 1);
        http_connection* conn = create_http_connection(ios);
        lua_utility::register_object(L, conn, "xul.http_connection");
        return 1;
    }
#if 1
    static int create_concurrent(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, 1);
        http_connection* conn = create_concurrent_http_connection(ios);
        lua_utility::register_object(L, conn, "xul.http_connection");
        return 1;
    }
#endif
    static int download(lua_State* L)
    {
        http_connection* self = get_self(L);
        std::string urlstr;
        if (!lua_utility::try_get_string(L, 2, &urlstr))
            return luaL_error(L, "xul.http_connection invalid url");
        int64_t pos = lua_utility::get_integer(L, 3, -1);
        int64_t len = lua_utility::get_integer(L, 4, -1);
        const string_table* headers = lua_utility::get_object<string_table>(L, 5);
        bool ret = self->download(urlstr.c_str(), pos, len, headers);
        lua_utility::push_bool(L, ret);
        return 1;
    }
    static int set_handler(lua_State* L)
    {
        http_connection* self = get_self(L);
        boost::intrusive_ptr<http_connection_handler> handler(new lua_http_connection_handler(L));
        self->set_handler(handler.get());
        return 0;
    }
    static int set_receive_buffer_size(lua_State* L)
    {
        http_connection* self = get_self(L);
        int bufsize = lua_utility::get_integer(L, 2, -1);
        if (bufsize <= 0)
            return 0;
        self->set_receive_buffer_size(bufsize);
        return 0;
    }
    static int close(lua_State* L)
    {
        http_connection* self = get_self(L);
        self->close();
        return 0;
    }
    static int pause(lua_State* L)
    {
        http_connection* self = get_self(L);
        self->pause();
        return 0;
    }
    static int resume(lua_State* L)
    {
        http_connection* self = get_self(L);
        self->resume();
        return 0;
    }
    static int is_started(lua_State* L)
    {
        http_connection* self = get_self(L);
        bool ret = self->is_started();
        lua_utility::push_bool(L, ret);
        return 1;
    }
    static int is_paused(lua_State* L)
    {
        http_connection* self = get_self(L);
        bool ret = self->is_paused();
        lua_utility::push_bool(L, ret);
        return 1;
    }
    static int get_options(lua_State* L)
    {
        http_connection* self = get_self(L);
        http_connection_options* opts = self->ref_options();
        lua_utility::register_object(L, opts, "xul.options");
        return 1;
    }
};


}
