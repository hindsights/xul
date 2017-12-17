#pragma once

#include <xul/net/http/http_server.hpp>
#include <xul/lua/lua_callback.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/lua.hpp>


namespace xul {


#define XUL_HTTP_SERVER "xul.http_server"
#define XUL_SIMPLE_HTTP_SERVER_ROUTER "xul.simple_http_server_router"

inline url_handler* create_lua_url_handler(lua_State* L)
{
    assert(lua_istable(L, -1));
    url_handler* handler = new lua_url_handler(L);
    return handler;
}

class lua_http_server_router : public xul::object_impl<http_server_router>
{
public:
    explicit lua_http_server_router(lua_State* L) : m_callback(L)
    {
        XUL_LOGGER_INIT("lua_http_server_router");
        XUL_DEBUG("new");
        XUL_DEBUG("luaL_ref " << m_callback.get_callback_ref());
    }
    ~lua_http_server_router()
    {
        XUL_DEBUG("delete");
    }

    virtual url_handler* create_url_handler(http_server* server, url_session* session, const url_request* req)
    {
        XUL_DEBUG("create_url_handler " << req->get_url());
        lua_State* L = m_callback.get_lua_state();
        m_callback.prepare_function_invoke();
        lua_utility::register_object(L, session, "xul.url_session");
        lua_utility::register_object(L, const_cast<url_request*>(req), "xul.url_request");
        lua_utility::dump_stack(L, "create_url_handler before invoke");
        int ret = lua_utility::pcall(L, 3);
        lua_utility::dump_stack(L, "create_url_handler before invoke1");
        if (ret < 0)
        {
            XUL_DEBUG("create_url_handler failed " << ret);
            return NULL;
        }
        lua_utility::dump_stack(L, "create_url_handler after invoke");
        return create_lua_url_handler(L);
    }

private:
    XUL_LOGGER_DEFINE();
    lua_callback m_callback;
};

class lua_http_server_route_handler : public xul::object_impl<http_server_route_handler>
{
public:
    explicit lua_http_server_route_handler(lua_State* L) : m_callback(L)
    {
        XUL_LOGGER_INIT("lua_http_server_route_handler");
        XUL_DEBUG("new");
        XUL_DEBUG("luaL_ref " << m_callback.get_callback_ref());
    }
    ~lua_http_server_route_handler()
    {
        XUL_DEBUG("delete");
    }

    virtual url_handler* create_url_handler()
    {
        XUL_DEBUG("create_url_handler");
        lua_State* L = m_callback.get_lua_state();
        m_callback.prepare_function_invoke();
        lua_utility::dump_stack(L, "create_url_handler before invoke");
        int ret = lua_utility::pcall(L, 0);
        lua_utility::dump_stack(L, "create_url_handler before invoke1");
        if (ret < 0)
        {
            XUL_DEBUG("create_url_handler failed " << ret);
            return NULL;
        }
        lua_utility::dump_stack(L, "create_url_handler after invoke");
        return create_lua_url_handler(L);
    }
private:
    XUL_LOGGER_DEFINE();
    lua_callback m_callback;
};

class lua_simple_http_server_router
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_simple_http_server_router::create },
            { "add", &lua_simple_http_server_router::add },
            { "set_not_found_handler", &lua_simple_http_server_router::set_not_found_handler },
            { NULL, NULL },
        };
        return the_methods;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, XUL_SIMPLE_HTTP_SERVER_ROUTER, get_methods());
    }

    static int create(lua_State* L)
    {
        simple_http_server_router* router = create_simple_http_server_router();
        xul::lua_utility::register_object(L, router, XUL_SIMPLE_HTTP_SERVER_ROUTER);
        return 1;
    }

    static int add(lua_State* L)
    {
        XUL_APP_DEBUG("lua_simple_http_server_router.add");
        simple_http_server_router* router = lua_utility::get_object<simple_http_server_router>(L, 1);
        if (!router)
            return -1;
        assert(lua_isstring(L, 2));
        assert(lua_isfunction(L, 3));
        std::string pattern = lua_utility::get_string(L, 2);
        if (pattern.empty())
        {
            XUL_APP_ERROR("lua_simple_http_server_router.add: pattern is empty");
            return -1;
        }
        http_server_route_handler* handler = new lua_http_server_route_handler(L);
        router->add(pattern.c_str(), handler);
        return 0;
    }
    static int set_not_found_handler(lua_State* L)
    {
        XUL_APP_DEBUG("lua_simple_http_server_router.set_not_found_handler");
        simple_http_server_router* router = lua_utility::get_object<simple_http_server_router>(L, 1);
        if (!router)
            return -1;
        assert(lua_isfunction(L, 2));
        http_server_route_handler* handler = new lua_http_server_route_handler(L);
        router->set_not_found_handler(handler);
        return 0;
    }
};


class lua_http_server
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_http_server::create },
            { "set_router", &lua_http_server::set_router },
            { "start", &lua_http_server::start },
            { "stop", &lua_http_server::stop },
            { "enable_reuse_address", &lua_http_server::enable_reuse_address },
            { "get_session_count", &lua_http_server::get_session_count },
            //{ "feed_client", &lua_http_server::feed_client },
            { NULL, NULL },
        };
        return the_methods;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, XUL_HTTP_SERVER, get_methods());
    }

    static int create(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, 1);
        http_server* svr = create_http_server(ios);
        xul::lua_utility::register_object(L, svr, XUL_HTTP_SERVER);
        return 1;
    }

    static int set_router(lua_State* L)
    {
        XUL_APP_DEBUG("set_router");
        http_server* svr = lua_utility::get_object<http_server>(L, 1);
        if (!svr)
            return -1;
        if (lua_isfunction(L, 2))
        {
            http_server_router* router = new lua_http_server_router(L);
            svr->set_router(router);
        }
        else
        {
            assert(lua_isuserdata(L, 2));
            http_server_router* router = lua_utility::get_object<http_server_router>(L, 2);
            assert(router);
            svr->set_router(router);
        }
        return 0;
    }

    static int start(lua_State* L)
    {
        http_server* svr = lua_utility::get_object<http_server>(L, 1);
        if (!svr)
            return -1;
        int port = lua_tointeger(L, 2);
        if (port < 0)
            return -1;
        bool ret = svr->start(port);
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
    static int stop(lua_State* L)
    {
        http_server* svr = lua_utility::get_object<http_server>(L, 1);
        if (!svr)
            return -1;
        svr->stop();
        return 0;
    }
    static int enable_reuse_address(lua_State* L)
    {
        http_server* svr = lua_utility::get_object<http_server>(L, 1);
        if (!svr)
            return -1;
        int enabled = lua_toboolean(L, 2);
        svr->enable_reuse_address(!!enabled);
        return 0;
    }

    static int get_session_count(lua_State* L)
    {
        http_server* svr = lua_utility::get_object<http_server>(L, 1);
        if (!svr)
            return -1;
        int ret = svr->get_session_count();
        lua_pushinteger(L, ret);
        return 1;
    }

    static int feed_client(lua_State* L)
    {
        http_server* svr = lua_utility::get_object<http_server>(L, 1);
        if (!svr)
            return -1;
        tcp_socket* sock = lua_utility::get_object<tcp_socket>(L, 2);
        bool ret = svr->feed_client(sock);
        lua_pushboolean(L, ret ? 1 : 0);
        return 1;
    }
};


}
