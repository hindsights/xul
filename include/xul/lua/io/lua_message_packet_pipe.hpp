#pragma once

#include <xul/io/codec/message_packet_pipe.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_message_packet_pipe
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] = 
        {
            { "new", &lua_message_packet_pipe::create }, 
			{ "feed", &lua_message_packet_pipe::feed }, 
			{ "read", &lua_message_packet_pipe::read }, 
            { NULL, NULL }, 
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.message_pipe", get_methods());
    }

    static int create(lua_State* L)
    {
		bool isBigEndian;
		if (!lua_utility::try_get_bool(L, 1, &isBigEndian))
			return -1;
		int maxsize;
		message_packet_pipe * pipe;
		if (!lua_utility::try_get_integer(L, 2, &maxsize))
			pipe = new message_packet_pipe(isBigEndian);
		else
			pipe = new message_packet_pipe(isBigEndian, maxsize);
        lua_utility::register_object(L, pipe, "xul.message_pipe");
        return 1;
    }
	
	static int feed(lua_State* L)
	{
		message_packet_pipe* self = lua_utility::get_object<message_packet_pipe>(L, 1);
		const uint8_t* data = static_cast<const uint8_t*>(lua_utility::get_userdata(L, 2, NULL));
		int size = lua_utility::get_integer(L, 3, -1);
		if(size < 0)
			assert(false);
		self->feed(data, size);
		return 0;		
	}

	static int read(lua_State* L)
	{
		message_packet_pipe* self = lua_utility::get_object<message_packet_pipe>(L, 1);
		int size = lua_utility::get_integer(L, 2, -1);
		const uint8_t* data = self->read(&size);
		//std::string str;
		//str.assign(size, data);
		//lua_pushstring(L,str);
		//const char *testdata = reinterpret_cast<const char*>(data);
		lua_pushlstring(L, reinterpret_cast<const char*>(data), size);
		//lua_pushlightuserdata(L, const_cast<uint8_t*>(data));
		lua_pushnumber(L, size);
		return 2;
	}
};

}
