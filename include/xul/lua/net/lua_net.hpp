#pragma once

#include <xul/lua/lang/lua_object.hpp>
#include <xul/lua/net/lua_io_service.hpp>
#include <xul/lua/net/lua_uri.hpp>
#include <xul/lua/net/lua_tcp_socket.hpp>
#include <xul/lua/net/lua_tcp_acceptor.hpp>
#include <xul/lua/net/lua_socket_address.hpp>
#include <xul/lua/net/lua_url_session.hpp>
#include <xul/lua/net/lua_url_handler.hpp>
#include <xul/lua/net/lua_url_request.hpp>
#include <xul/lua/net/lua_url_response.hpp>
#include <xul/lua/net/http/lua_http_connection.hpp>
#include <xul/lua/net/http/lua_http_client.hpp>
#include <xul/lua/net/http/lua_http_server.hpp>
#include <xul/lua/net/lua_inet4_address.hpp>


namespace xul {


inline void register_lua_net(lua_State* L)
{
    lua_object::register_class(L);
    lua_io_service::register_class(L);
    lua_timer::register_class(L);
    lua_uri::register_class(L);
    lua_tcp_socket::register_class(L);
    lua_tcp_acceptor::register_class(L);
	lua_socket_address::register_class(L);
	lua_inet4_address::register_class(L);
    lua_ipv4::register_class(L);
    lua_url_session::register_class(L);
    lua_url_handler::register_class(L);
    lua_url_request::register_class(L);
    lua_url_response::register_class(L);
    lua_url_message::register_class(L);
    lua_http_connection::register_class(L);
    lua_http_client::register_class(L);
    lua_http_server::register_class(L);
    lua_simple_http_server_router::register_class(L);
}


}
