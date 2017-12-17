#pragma once

#include <xul/text/hex_encoding.hpp>
#include <xul/text/base64_encoding.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_hex_encoding
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] = 
        {
            { "decode", &decode }, 
            { "encode", &encode },
            { NULL, NULL }, 
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.hex_encoding", get_methods());
    }

    static int decode(lua_State* L)
    {
        std::string s = lua_utility::get_string(L, 1);
        std::string ret = xul::hex_encoding::decode(s);
        lua_pushlstring(L, ret.c_str(), ret.size());
        return 1;
    }

    static int encode(lua_State* L)
    {
        std::string s = lua_utility::get_string(L, 1);
        //xul::hex_encoding h;
        //std::string r_s = h.encode(s);
        std::string ret = xul::hex_encoding::lower_case().encode(s);
        lua_pushlstring(L, ret.c_str(), ret.size());
        return 1;
    }
};


class lua_base64
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "encode", &encode },
            { "decode", &decode },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.base64", get_methods());
    }

    static int encode(lua_State* L)
    {
        std::string s = lua_utility::get_string(L, 1);
        std::string s2 = base64_encoding::encode(s);
        lua_pushlstring(L, s2.c_str(), s2.size());
        return 1;
    }
    static int decode(lua_State* L)
    {
        std::string s = lua_utility::get_string(L, 1);
        std::string s2 = base64_encoding::decode(s);
        lua_pushlstring(L, s2.c_str(), s2.size());
        return 1;
    }
};


}
