#pragma once

#include <xul/net/inet4_address.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


	class lua_inet4_address
	{
	public:
		static const luaL_Reg* get_methods()
		{
			static const luaL_Reg url_request_functions[] = 
			{
                { "get_host_address", &lua_inet4_address::get_host_address }, 
                { "to_string", &lua_inet4_address::to_string }, 
				{ NULL, NULL }, 
			};
			return url_request_functions;
		}

		static void register_class(lua_State* L)
		{
			lua_utility::register_object_class(L, "xul.inet4_address", get_methods());
		}

		static int get_host_address(lua_State* L)
		{
			xul::inet4_address addr;
			std::string str = lua_utility::get_string(L, 1);
			addr.set_address(str.c_str());
			unsigned long data = addr.get_address();
			lua_pushnumber(L, data);
			return 1;
		}

        static int to_string(lua_State* L)
        {
            xul::inet4_address addr;
            uint32_t val = xul::lua_utility::get_integer(L, 1, 0);
            addr.set_address(val);
            lua_pushstring(L, addr.str().c_str());
            return 1;
        }

	};

}
