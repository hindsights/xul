#pragma once

#include <xul/data/string_tokenizer.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


#define XUL_STRING_TOKENIZER "xul.string_tokenizer"
class lua_string_tokenizer
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_functions[] =
        {
            { "new", &lua_string_tokenizer::create },
            { "reset", &lua_string_tokenizer::reset },
            { "set_max_buffer_size", &lua_string_tokenizer::set_max_buffer_size },
            { "set_delimiter", &lua_string_tokenizer::set_delimiter },
            { "set_include_delimiter", &lua_string_tokenizer::set_include_delimiter },
            { "is_delimiter_included", &lua_string_tokenizer::is_delimiter_included },
            { "feed", &lua_string_tokenizer::feed },
            { "next_token", &lua_string_tokenizer::next_token },
            { "next_token_buffer", &lua_string_tokenizer::next_token_buffer },
            { "get_remains", &lua_string_tokenizer::get_remains },
            { "get_remains_buffer", &lua_string_tokenizer::get_remains_buffer },
            { NULL, NULL },
        };
        return the_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, XUL_STRING_TOKENIZER, get_methods());
    }

    static int create(lua_State* L)
    {
        string_tokenizer* buf = create_string_tokenizer();
        lua_utility::register_object(L, buf, XUL_STRING_TOKENIZER);
        return 1;
    }
    static string_tokenizer* get_self(lua_State* L, int idx = 1)
    {
        string_tokenizer* self = lua_utility::get_object<string_tokenizer>(L, 1);
        assert(self);
        return self;
    }
    static int reset(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        self->reset();
        return 0;
    }
    static int set_max_buffer_size(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        int bufsize = lua_utility::get_integer(L, 2, -1);
        if (bufsize <= 0)
            return -2;
        self->set_max_buffer_size(bufsize);
        return 0;
    }
    static int set_delimiter(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        size_t delimiterSize = 0;
        const char* delimiter = lua_tolstring(L, 2, &delimiterSize);
        if (NULL == delimiter || delimiterSize <= 0)
            return -2;
        self->set_delimiter(delimiter, delimiterSize);
        return 0;
    }
    static int set_include_delimiter(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        bool val = false;
        if (!lua_utility::try_get_bool(L, 2, &val))
        {
            assert(false);
            return -2;
        }
        self->set_include_delimiter(val);
        return 0;
    }
    static int is_delimiter_included(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        lua_utility::push_bool(L, self->is_delimiter_included());
        return 1;
    }
    static int feed(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        int argtype = lua_type(L, 2);
        if (!lua_isuserdata(L, 2) || !lua_isnumber(L, 3))
        {
            assert(false);
            return -2;
        }
        const void* buf = lua_touserdata(L, 2);
        int bufsize = lua_tonumber(L, 3);
        if (NULL != buf && bufsize > 0)
        {
            self->feed(static_cast<const char*>(buf), bufsize);
        }
        return 0;
    }
    static int next_token(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        const char* token = NULL;
        size_t tokensize = 0;
        bool ret = self->next_token(&token, &tokensize);
        if (ret)
        {
            lua_pushlstring(L, const_cast<char*>(token), tokensize);
            return 1;
        }
        lua_pushnil(L);
        return 1;
    }
    static int get_remains(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        const char* token = NULL;
        size_t tokensize = 0;
        bool ret = self->get_remains(&token, &tokensize);
        if (ret)
        {
            lua_pushlstring(L, const_cast<char*>(token), tokensize);
            return 1;
        }
        lua_pushnil(L);
        return 1;
    }
    static int next_token_buffer(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        const char* token = NULL;
        size_t tokensize = 0;
        bool ret = self->next_token(&token, &tokensize);
        if (ret)
        {
            lua_pushlstring(L, token, tokensize);
            return 1;
        }
        lua_pushnil(L);
        return 1;
    }
    static int get_remains_buffer(lua_State* L)
    {
        string_tokenizer* self = get_self(L);
        if (!self)
            return -1;
        const char* token = NULL;
        size_t tokensize = 0;
        bool ret = self->get_remains(&token, &tokensize);
        if (ret)
        {
            lua_pushlstring(L, token, tokensize);
            return 1;
        }
        lua_pushnil(L);
        return 1;
    }
};


}
