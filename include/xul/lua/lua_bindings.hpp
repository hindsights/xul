#pragma once

#include <xul/lua/net/lua_net.hpp>
#include <xul/lua/io/lua_structured_writer.hpp>
#include <xul/lua/io/lua_message_packet_pipe.hpp>
#include <xul/lua/lua_encoding.hpp>
#include <xul/lua/data/lua_string.hpp>
#include <xul/lua/data/lua_block_pool.hpp>
#include <xul/lua/data/lua_buffer.hpp>
#include <xul/lua/data/lua_byte_order.hpp>
#include <xul/lua/data/lua_numerics.hpp>
#include <xul/lua/data/lua_string_tokenizer.hpp>
#include <xul/lua/util/lua_options.hpp>
#include <xul/lua/util/lua_time_counter.hpp>
#include <xul/lua/log/lua_logger.hpp>
#include <xul/lua/log/lua_log_manager.hpp>
#include <xul/lua/hash/lua_hasher.hpp>
#include <xul/lua/util/lua_struct.hpp>
#include <xul/lua/lua_os.hpp>


namespace xul {


inline void register_lua(lua_State* L)
{
    lua_logger::register_class(L);
    lua_log_manager::register_class(L);
    lua_structured_writer::register_class(L);
	lua_message_packet_pipe::register_class(L);
    lua_options::register_class(L);
    lua_base64::register_class(L);
	lua_hex_encoding::register_class(L);
	lua_hasher::register_class(L);
    lua_string::register_class(L);
	lua_byte_order::register_class(L);
    lua_buffer::register_class(L);
	lua_numerics::register_class(L);
    lua_block_pool::register_class(L);
    lua_string_tokenizer::register_class(L);
	lua_struct::register_class(L);
    register_lua_net(L);
    lua_time_counter::register_functions(L);
    lua_paths::register_class(L);
    lua_utility::register_null(L);
}


}
