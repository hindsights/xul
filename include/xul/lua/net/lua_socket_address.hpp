#pragma once

#include <xul/net/tcp_socket.hpp>
#include <xul/net/inet_socket_address.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {

class lua_socket_address
{
public:
	static const luaL_Reg* get_methods()
	{
		static const luaL_Reg url_request_functions[] = 
		{
			{ NULL, NULL }, 
		};
		return url_request_functions;
	}

	static void register_class(lua_State* L)
	{
		// lua_utility::register_object_class(L, "xul.socket_address", get_methods());
    }
    static void push_inet_socket_address(lua_State* L, const inet_socket_address& addr)
    {
        lua_newtable(L);
        lua_pushnumber(L, addr.get_ip());
        lua_utility::dump_stack(L, "push_inet_socket_address");
        lua_setfield(L, -2, "ip");
        lua_utility::dump_stack(L, "push_inet_socket_address");
        lua_pushnumber(L, addr.get_port());
        lua_setfield(L, -2, "port");
        lua_utility::dump_stack(L, "push_inet_socket_address");
    }

};

class lua_ipv4
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "tostring", &lua_ipv4::tostring },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.ipv4", get_methods());
    }

    static int tostring(lua_State* L)
    {
        lua_Number val = lua_tonumber(L, -1);
        inet4_address addr;
        addr.set_raw_address(val);
        lua_pushstring(L, addr.str().c_str());
        return 1;
    }
};


}
