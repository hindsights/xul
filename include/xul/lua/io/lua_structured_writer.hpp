#pragma once

#include <xul/io/structured_writer.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_string_output_stream
{
public:
    class string_output_stream : public object_impl<object>
    {
    public:
        std::ostringstream os;
        ostream_stream oss;

        explicit string_output_stream() : oss(os)
        {
        }
    };
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "new", &lua_string_output_stream::create },
            { "get_string", &lua_string_output_stream::get_string },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.string_output_stream", get_methods());
    }

    static int create(lua_State* L)
    {
        string_output_stream* obj = new string_output_stream;
        lua_utility::register_object(L, obj, "xul.string_output_stream");
        return 1;
    }

    static int get_string(lua_State* L)
    {
        string_output_stream* obj = lua_utility::get_object<string_output_stream>(L, -1);
        if (!obj)
            return -1;
        std::string s = obj->os.str();
        lua_pushlstring(L, s.c_str(), s.size());
        return 1;
    }
};


class lua_structured_writer
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg url_request_functions[] =
        {
            { "start_list", &lua_structured_writer::start_list },
            { "end_list", &lua_structured_writer::end_list },
            { "start_dict", &lua_structured_writer::start_dict },
            { "end_dict", &lua_structured_writer::end_dict },
            { "write_item", &lua_structured_writer::write_item },
            { "write", &lua_structured_writer::write_item },
            // { "write_value", &lua_structured_writer::write_value },
            { NULL, NULL },
        };
        return url_request_functions;
    }

    static const luaL_Reg* get_xul_methods()
    {
        static const luaL_Reg xul_methods[] =
        {
            { "create_xml_writer", &lua_create_xml_writer },
            { "create_json_writer", &lua_create_json_writer },
            { NULL, NULL },
        };
        return xul_methods;
    }

    static void register_class(lua_State* L)
    {
        lua_string_output_stream::register_class(L);
        lua_utility::register_object_class(L, "xul.structured_writer", get_methods());
        //luaL_register(L, "xul", get_xul_methods());
        lua_utility::register_functions(L, "xul", get_xul_methods());
    }

    static int lua_create_writer(lua_State* L, structured_writer* creator(output_stream*, bool))
    {
        lua_string_output_stream::string_output_stream* os = lua_utility::get_object<lua_string_output_stream::string_output_stream>(L, 1);
        if (!os)
            return -1;
        bool compressed = lua_utility::get_bool(L, 2, false);
        structured_writer* writer = creator(&os->oss, compressed);
        lua_utility::register_object(L, writer, "xul.structured_writer");
        return 1;
    }

    static int lua_create_xml_writer(lua_State* L)
    {
        return lua_create_writer(L, &create_xml_writer);
    }

    static int lua_create_json_writer(lua_State* L)
    {
        return lua_create_writer(L, &create_json_writer);
    }

    static int start_dict(lua_State* L)
    {
        structured_writer* obj = lua_utility::get_object<structured_writer>(L, 1);
        if (!obj)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        obj->start_dict(s);
        return 0;
    }
    static int end_dict(lua_State* L)
    {
        structured_writer* obj = lua_utility::get_object<structured_writer>(L, 1);
        if (!obj)
            return -1;
        obj->end_dict();
        return 0;
    }
    static int start_list(lua_State* L)
    {
        structured_writer* obj = lua_utility::get_object<structured_writer>(L, 1);
        if (!obj)
            return -1;
        const char* s = lua_tostring(L, 2);
        if (!s)
            return -1;
        int count = lua_tointeger(L, 3);
        obj->start_list(s, count);
        return 0;
    }
    static int end_list(lua_State* L)
    {
        structured_writer* obj = lua_utility::get_object<structured_writer>(L, 1);
        if (!obj)
            return -1;
        obj->end_list();
        return 0;
    }
    static int write_value(lua_State* L)
    {
        structured_writer* obj = lua_utility::get_object<structured_writer>(L, 1);
        if (!obj)
            return -1;
        int type = lua_type(L, 2);
        switch (type)
        {
        case LUA_TBOOLEAN:
            obj->write_bool_value(!!lua_toboolean(L, 2));
            break;
        case LUA_TNUMBER:
            obj->write_int64_value(lua_tonumber(L, 2));
            break;
        case LUA_TSTRING:
            {
                size_t strlen = 0;
                const char* val = lua_tolstring(L, 2, &strlen);
                obj->write_string_value(val ? std::string(val, strlen).c_str() : "");
            }
            break;
        default:
            XUL_APP_DEBUG("write_value invalid value " << type);
            assert(false);
            break;
        }
        return 0;
    }
    static int write_item(lua_State* L)
    {
        structured_writer* obj = lua_utility::get_object<structured_writer>(L, 1);
        if (!obj)
            return -1;
        const char* key = lua_tostring(L, 2);
        if (!key)
            return -1;
            int type = lua_type(L, 3);
            switch (type)
            {
            case LUA_TBOOLEAN:
                obj->write_bool(key, !!lua_toboolean(L, 3));
                break;
            case LUA_TNUMBER:
                obj->write_number(key, lua_tonumber(L, 3));
                break;
            case LUA_TSTRING:
                {
                    size_t strlen = 0;
                    const char* val = lua_tolstring(L, 3, &strlen);
                    obj->write_string(key, val ? std::string(val, strlen).c_str() : "");
                }
                break;
            default:
                XUL_APP_DEBUG("write_element invalid value " << type);
                assert(false);
                break;
            }
            return 0;
    }
};


}
