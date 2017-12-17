#pragma once

#include <xul/net/io_service.hpp>
#include <xul/net/tcp_acceptor.hpp>
#include <xul/net/tcp_acceptor_listener.hpp>
#include <xul/net/inet_socket_address.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/net/lua_socket_address.hpp>
#include <xul/log/log.hpp>

namespace xul {

class lua_tcp_acceptor_handler : public object_impl<tcp_acceptor_handler>
{
public:
    explicit lua_tcp_acceptor_handler(lua_State* L) : m_callback(L)
    {
        XUL_LOGGER_INIT("pplite.tcpAcceptorListener");
        XUL_DEBUG("new");
    }
    virtual ~lua_tcp_acceptor_handler()
    {
        XUL_DEBUG("delete");
    }
    virtual void on_acceptor_client(tcp_acceptor* acceptor, tcp_socket* newClient, const inet_socket_address& sockAddr)
    {
        XUL_DEBUG("on_acceptor_client"<< acceptor << " " << newClient << " "<< sockAddr);
        lua_State* L = m_callback.get_lua_state();
        xul::lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("on_acceptor_client"))
        {
            XUL_WARN("on_acceptor_client_failed ignored");
            return;
        }
		lua_utility::register_object(L, acceptor, "xul.tcp_acceptor");
		lua_utility::register_object(L, newClient, "xul.tcp_socket");
		lua_socket_address::push_inet_socket_address(L, sockAddr);
        xul::lua_utility::pcall(L, 3);
    }
    virtual void on_acceptor_error(tcp_acceptor* acceptor, int errcode)
    {
        XUL_DEBUG("on_acceptor_error" << acceptor << " "<< errcode);
        lua_State* L = m_callback.get_lua_state();
        xul::lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("on_acceptor_error"))
        {
            XUL_WARN("on_acceptor_error_failed ignored");
            return;
        }
		lua_utility::register_object(L, acceptor, "xul.tcp_acceptor");
        lua_pushnumber(L, errcode);
        xul::lua_utility::pcall(L, 2);
    }
private:
    XUL_LOGGER_DEFINE();
    xul::lua_callback m_callback;
};

class lua_tcp_acceptor
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] = 
        {
			{ "new", &lua_tcp_acceptor::create },
            { "reset_listener", &lua_tcp_acceptor::reset_listener },
            { "set_handler", &lua_tcp_acceptor::set_handler },
            { "open", &lua_tcp_acceptor::open },
            { "close", &lua_tcp_acceptor::close },
            { NULL, NULL }, 
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.tcp_acceptor", get_methods());
    }

    static tcp_acceptor* get_self(lua_State* L)
    {
        tcp_acceptor* self = lua_utility::get_object<tcp_acceptor>(L, 1);
        assert(self);
        return self;
    }

	static int create(lua_State* L)
	{
		io_service* ios = lua_utility::get_object<io_service>(L, 1);
		if(!ios)
			return -1;
        tcp_acceptor* acc = ios->create_tcp_acceptor();
		lua_utility::register_object(L, acc, "xul.tcp_acceptor");
		return 1;
	}

	static int set_handler(lua_State* L)
	{
        tcp_acceptor* self = get_self(L);
        if(!self)
            return -1;
        tcp_acceptor_handler* handler = new lua_tcp_acceptor_handler(L);
        self->set_listener(handler);
        return 0;
	}

    static int reset_listener(lua_State* L)
    {
        tcp_acceptor* self = get_self(L);
        if(!self)
            return -1;
        self->reset_listener();
        return 0;
    }

    static int open(lua_State* L)
    {
        tcp_acceptor* self = get_self(L);
        uint16_t port = lua_utility::get_integer(L, 2, -1);
        if(port < 0)
            assert(false);
        bool reuseAddr = lua_utility::get_bool(L, 3, false);
        lua_pushboolean(L, self->open(port, reuseAddr));
        return 1;
    }

    static int close(lua_State* L)
    {
        tcp_acceptor* self = get_self(L);
        if(!self)
            return -1;
        self->close();
        return 0;
    }
};

}
