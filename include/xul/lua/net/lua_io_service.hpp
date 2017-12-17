#pragma once

#include <xul/libs/asio/asio_tcp_socket.hpp>
#include <xul/libs/asio/asio_tcp_acceptor.hpp>
#include <xul/lua/util/lua_timer.hpp>
#include <xul/net/io_service.hpp>
#include <xul/lua/lua.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


#define XUL_IO_SERVICE "xul.io_service"


class lua_io_service
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_io_service::create },
            { "start", &lua_io_service::start },
            { "wait", &lua_io_service::wait },
            { "stop", &lua_io_service::stop },
            { "create_periodic_timer", &lua_io_service::create_periodic_timer },
            { "create_once_timer", &lua_io_service::create_once_timer },
			{ "create_tcp_socket", &lua_io_service::create_tcp_socket },
            { "create_tcp_acceptor", &lua_io_service::create_tcp_acceptor },
            { NULL, NULL },
        };
        return the_methods;
    }
    static const luaL_Reg* get_metatable()
    {
        static const luaL_Reg the_metatable[] = 
        {
            { "__gc", &lua_io_service::destroy }, 
            { NULL, NULL }, 
        };
        return the_metatable;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_class(L, XUL_IO_SERVICE, get_metatable(), get_methods());
    }

    static int create(lua_State* L)
    {
        io_service* ios = create_io_service();
        lua_utility::register_object(L, ios, XUL_IO_SERVICE);
        return 1;
    }
    static int destroy(lua_State* L)
    {
        return lua_utility::destroy_object(L);
    }
    static int wait(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        if (!ios)
            return -1;
        ios->wait();
        return 0;
    }
    static int stop(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        if (!ios)
            return -1;
        ios->stop();
        return 0;
    }
    static int start(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        if (!ios)
            return -1;
        ios->start();
        return 0;
    }
    static int create_periodic_timer(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        if (!ios)
            return -1;
        timer* t = ios->create_periodic_timer();
        lua_utility::register_object(L, t, "xul.timer");
        return 1;
    }
    static int create_once_timer(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        if (!ios)
            return -1;
        timer* t = ios->create_once_timer();
        lua_utility::register_object(L, t, "xul.timer");
        return 1;
    }
	static int create_tcp_socket(lua_State* L)
	{
		io_service* ios = lua_utility::get_object<io_service>(L, -1);
		if(!ios)
			return -1;
		tcp_socket* s = ios->create_tcp_socket();
		lua_utility::register_object(L, s, "xul.tcp_socket");
		return 1;
	}

    static int create_tcp_acceptor(lua_State* L)
    {
        io_service* ios = lua_utility::get_object<io_service>(L, -1);
        if(!ios)
            return -1;
        tcp_acceptor* acc = ios->create_tcp_acceptor();
        lua_utility::register_object(L, acc, "xul.tcp_acceptor");
        return 1;
    }
};

}
