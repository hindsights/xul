#pragma once

#include <xul/lua/lua_callback.hpp>
#include <xul/net/url_handler.hpp>
#include <xul/net/url_messages.hpp>
#include <xul/data/printables.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/lua/lua_stack.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_url_handler : public object_impl<url_handler>
{
public:
    explicit lua_url_handler(lua_State* state) : m_callback(state)
    {
        XUL_LOGGER_INIT("url_handler");
        XUL_DEBUG("new");
    }
    ~lua_url_handler()
    {
        XUL_DEBUG("delete");
    }

    virtual void handle_request(url_session* session, const url_request* req)
    {
        XUL_DEBUG("handle_request " << *req);
        //luaL_dostring(m_state, "print(my_handler)");
        //luaL_dostring(m_state, "for k,v in pairs(my_handler) do\n""print(\"Global key\", k, \"value\", v)\n""end\n");
        //luaL_dostring(m_state, "print(my_handler.handle_request)");
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        lua_utility::dump_stack(L, "handle_request");
        if (!m_callback.prepare_object_invoke("handle_request"))
        {
            assert(false);
            return;
        }
        lua_utility::dump_stack(L, "handle_request-invoke0");
		lua_utility::register_object(L, session, "xul.url_session");
		lua_utility::register_object(L, const_cast<url_request*>(req), "xul.url_request");
        lua_utility::dump_stack(L, "handle_request-invoke1");
        lua_utility::pcall(L, 2);
    }
    virtual void handle_body_data(url_session* session, const uint8_t* data, int size)
    {
        XUL_DEBUG("handle_body_data " << xul::make_tuple(data, size));
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("handle_body_data"))
        {
            session->close();
            return;
        }
        lua_utility::register_object(L, session, "xul.url_session");
        lua_pushlstring(L, reinterpret_cast<const char*>(data), size);
        lua_utility::pcall(L, 2);
    }
    virtual void handle_error(url_session* session, int errcode)
    {
        XUL_DEBUG("handle_error " << errcode);
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("handle_error"))
        {
            session->close();
            return;
        }
        lua_utility::register_object(L, session, "xul.url_session");
        lua_pushinteger(L, errcode);
        lua_utility::pcall(L, 2);
    }
    virtual void handle_data_sent(url_session* session, int bytes)
    {
        XUL_DEBUG("handle_data_sent " << bytes);
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("handle_data_sent"))
        {
            return;
        }
        lua_utility::register_object(L, session, "xul.url_session");
        lua_pushinteger(L, bytes);
        lua_utility::pcall(L, 2);
    }
    virtual void handle_request_finished(url_session* session)
    {
        XUL_DEBUG("handle_request_finished ");
        lua_State* L = m_callback.get_lua_state();
        lua_auto_stack_cleaner stack_cleaner(L);
        if (!m_callback.prepare_object_invoke("handle_request_finished"))
        {
            return;
        }
        lua_utility::register_object(L, session, "xul.url_session");
        lua_utility::pcall(L, 1);
    }

    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "new", &lua_url_handler::create },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.url_handler", get_methods());
    }

    static int create(lua_State* L)
    {
        url_handler* obj = new lua_url_handler(L);
        lua_utility::register_object(L, obj, "xul.url_handler");
        return 1;
    }

private:
    XUL_LOGGER_DEFINE();
    lua_callback m_callback;
};


}
