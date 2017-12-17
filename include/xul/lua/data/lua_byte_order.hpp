#pragma once

#include <xul/data/byte_order.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_byte_order
{
public:
	static const luaL_Reg* get_methods()
	{
		static const luaL_Reg the_methods[] = 
		{
			{ "convert_dword", &lua_byte_order::convert_dword }, 
			{ NULL, NULL }, 
		};
		return the_methods;
	}
	static void register_class(lua_State* L)
	{
		lua_utility::register_object_class(L, "xul.bytes_order", get_methods());
	}
	static int convert_dword(lua_State* L)
	{
		bool isbigendian = lua_utility::get_bool(L, 1, false);
		int val = lua_utility::get_integer(L, 2, -1);
		if(val < 0)
		{
			assert(false);
			return -1;
		}
		int ctval = xul::byte_order(isbigendian).convert_dword(val);
		lua_pushnumber(L, ctval);
		return 1;
	}
};


}
