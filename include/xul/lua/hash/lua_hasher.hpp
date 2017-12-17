#pragma once

#include <xul/hash/hasher.hpp>
#include <xul/data/big_number.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_hasher
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] = 
        {
			{"hash", &hash }, 
            { NULL, NULL }, 
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.hasher", get_methods());
        //luaL_register(L, "xul.hasher", get_methods());
    }
	
	static int hash(lua_State* L)
	{
		std::string s = lua_utility::get_string(L, 1);
		xul::sha1_hash sh = xul::hasher::hash(s);
		std::string s2 = sh.str();
		lua_pushlstring(L, s2.c_str(), s2.size());
		return 1;
	}
};

}
