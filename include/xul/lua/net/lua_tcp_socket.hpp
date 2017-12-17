#pragma once

#include <xul/net/io_service.hpp>
#include <xul/net/tcp_socket.hpp>
#include <xul/net/tcp_socket_listener.hpp>
#include <xul/net/inet_socket_address.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/net/lua_socket_address.hpp>
#include <xul/log/log.hpp>

namespace xul {

class lua_tcp_socket_handler : public object_impl<tcp_socket_handler>
{
public:
	explicit lua_tcp_socket_handler(lua_State* L) : m_callback(L)
	{
		XUL_LOGGER_INIT("pplite.tcpSocketListener");
		XUL_DEBUG("new");
	}
	virtual ~lua_tcp_socket_handler()
	{
		XUL_DEBUG("delete");
	}
	virtual void on_socket_connect(tcp_socket* sender)
	{
		XUL_DEBUG("on_socket_connect" << sender);
		lua_State* L = m_callback.get_lua_state();
		xul::lua_auto_stack_cleaner stack_cleaner(L);
		if (!m_callback.prepare_object_invoke("on_socket_connect"))
		{
			XUL_WARN("on_socket_connect ignored");
			return;
		}
		lua_utility::register_object(L, sender, "xul.tcp_socket");
		xul::lua_utility::pcall(L, 1);
	}
	virtual void on_socket_connect_failed(tcp_socket* sender, int errcode)
	{
		XUL_DEBUG("on_socket_connect" << sender << " " << errcode);
		lua_State* L = m_callback.get_lua_state();
		xul::lua_auto_stack_cleaner stack_cleaner(L);
		if (!m_callback.prepare_object_invoke("on_socket_connect_failed"))
		{
			XUL_WARN("on_socket_connect_failed ignored");
			return;
		}
		lua_utility::register_object(L, sender, "xul.tcp_socket");
		lua_pushnumber(L, errcode);
		xul::lua_utility::pcall(L, 2);
	}
	virtual void on_socket_receive(tcp_socket* sender, unsigned char* data, size_t size)
	{
		XUL_DEBUG("on_socket_receive"<< sender << " " << data << " " << size);
		lua_State* L = m_callback.get_lua_state();
		xul::lua_auto_stack_cleaner stack_cleaner(L);
		if (!m_callback.prepare_object_invoke("on_socket_receive"))
		{
			XUL_WARN("on_socket_receive ignored");
			return;
		}
		lua_utility::register_object(L, sender, "xul.tcp_socket");
		lua_pushlstring(L, reinterpret_cast<const char*>(data), size);
		xul::lua_utility::pcall(L, 2);
	}
	virtual void on_socket_receive_failed(tcp_socket* sender, int errcode)
	{
		XUL_DEBUG("on_socket_receive_failed"<< sender << " " << errcode);
		lua_State* L = m_callback.get_lua_state();
		xul::lua_auto_stack_cleaner stack_cleaner(L);
		if (!m_callback.prepare_object_invoke("on_socket_receive_failed"))
		{
			XUL_WARN("on_socket_receive_failed");
			return;
		}
		lua_utility::register_object(L, sender, "xul.tcp_socket");
        lua_pushnumber(L, errcode);
        xul::lua_utility::pcall(L, 2);
	}
private:
	XUL_LOGGER_DEFINE();
	xul::lua_callback m_callback;
};

class lua_tcp_socket
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] = 
        {
			{ "new", &lua_tcp_socket::create },
            { "get_remote_address", &lua_tcp_socket::get_remote_address }, 
            { "get_sending_queue_size", &lua_tcp_socket::get_sending_queue_size }, 
			{ "set_max_sending_queue_size", &lua_tcp_socket::set_max_sending_queue_size },
			{ "connect", &lua_tcp_socket::connect },
			{ "receive", &lua_tcp_socket::receive },
			{ "send", &lua_tcp_socket::send },
			{ "set_listener", &lua_tcp_socket::set_listener },
			{ "reset_listener", &lua_tcp_socket::reset_listener },
			{ "close", &lua_tcp_socket::close },
            { "destroy", &lua_tcp_socket::destroy },
            { NULL, NULL }, 
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.tcp_socket", get_methods());
    }

    static tcp_socket* get_self(lua_State* L)
    {
        tcp_socket* self = lua_utility::get_object<tcp_socket>(L, 1);
        assert(self);
        return self;
    }

	static int create(lua_State* L)
	{
		io_service* ios = lua_utility::get_object<io_service>(L, 1);
		if(!ios)
			return -1;
		tcp_socket* s = ios->create_tcp_socket();
		lua_utility::register_object(L, s, "xul.tcp_socket");
		return 1;
	}

	static int set_max_sending_queue_size(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		int maxsize = lua_utility::get_integer(L, -1, -1);
		if (maxsize < 0)
		{
			assert(false);
		}
		self->set_max_sending_queue_size(maxsize);
		return 0;
	}

    static int get_sending_queue_size(lua_State* L)
    {
        tcp_socket* self = get_self(L);
        if (!self)
            return -1;
        lua_pushnumber(L, self->get_sending_queue_size());
        return 1;
    }

    static int get_remote_address(lua_State* L)
    {
        tcp_socket* self = get_self(L);
        if (!self)
            return -1;
        xul::inet_socket_address addr;
        bool ret = self->get_remote_address(addr);
        if (!ret)
        {
            lua_pushnil(L);
            return 1;
		}
		lua_socket_address::push_inet_socket_address(L, addr);
        return 1;
    }

	static int set_listener(lua_State* L)
	{
		//may be any unstable
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		//boost::intrusive_ptr<tcp_socket_handler> handler(new lua_tcp_socket_handler(L));
		//self->set_listener(handler.get());
        assert(lua_istable(L, -1));
		tcp_socket_handler* handler = new lua_tcp_socket_handler(L);
		self->set_listener(handler);
		return 0;
	}

	static int reset_listener(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		self->reset_listener();
		return 0;
	}

	static int connect(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		//inet_socket_address* sockaddr = static_cast<xul::inet_socket_address*>(lua_utility::get_userdata(L, 2, NULL));
		uint32_t ip = lua_utility::get_integer(L, 2, -1);
		uint16_t port = lua_utility::get_integer(L, 3, -1);
		inet_socket_address addr(ip, port);
		self->connect(addr);
		return 0;
	}

	static int receive(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		int size = lua_utility::get_integer(L, 2, -1);
		if(size < 0)
			assert(false);
		self->receive(size);
		return 0;
	}

	static int send(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		size_t ld;
		const char *data = luaL_checklstring(L, 2, &ld);
        int size = ld;
		//int size = lua_utility::get_integer(L, 3, -1);
		assert(size > 0);
		bool ret = self->send(data, size);
		lua_utility::push_bool(L, ret);
		//lua_pushboolean(L, self->send(data, size));
		return 1;
	}

	static int close(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		self->close();
		return 0;
	}

	static int destroy(lua_State* L)
	{
		tcp_socket* self = get_self(L);
		if(!self)
			return -1;
		self->destroy();
		return 0;
	}

};

}
