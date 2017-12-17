#pragma once

#include <xul/data/numerics.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_numerics
{
public:
	static const luaL_Reg* get_methods()
	{
		static const luaL_Reg the_methods[] = 
		{
			{ "parse", &lua_numerics::parse }, 
			{ NULL, NULL }, 
		};
		return the_methods;
	}
	static void register_class(lua_State* L)
	{
		lua_utility::register_object_class(L, "xul.numerics", get_methods());
	}
	static int parse(lua_State* L)
	{
		std::string s = lua_utility::get_string(L, 1);
		int port = xul::numerics::parse<int>(s, 0);
		lua_pushnumber(L, port);
		return 1;
	}

};


}