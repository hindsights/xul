#pragma once

#include <xul/util/options.hpp>
#include <xul/util/options_utils.hpp>
#include <xul/lua/lua_utility.hpp>
#include <xul/data/string_table.hpp>
#include <xul/io/structured_writer.hpp>
#include <xul/util/dumpable.hpp>
#include <xul/log/log.hpp>


namespace xul {


class lua_options
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg options_functions[] =
        {
            { "new", &lua_options::create },
            { "set", &lua_options::set },
            { "update", &lua_options::update },
            { "dump", &lua_options::dump },
            { NULL, NULL },
        };
        return options_functions;
    }
    static const luaL_Reg* get_metatable()
    {
        static const luaL_Reg the_metatable[] =
        {
            { "__gc", &xul::lua_utility::destroy_object },
            { "__index", &lua_options::index_table},
            { NULL, NULL },
        };
        return the_metatable;
    }

    static const luaL_Reg* get_xul_methods()
    {
        static const luaL_Reg xul_methods[] =
        {
            { "create_dynamic_options", &lua_create_dynamic_options },
            { NULL, NULL },
        };
        return xul_methods;
    }

    static int lua_create_dynamic_options (lua_State* L)
    {
        options* opts = create_dynamic_options();
        lua_utility::register_object(L, opts, "xul.options");
        return 1;
    }

    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.options", get_methods());
        lua_utility::register_table_class(L, "xul.options_table", get_metatable(), get_methods());
        lua_utility::register_function(L, "xul", "create_dynamic_options", &lua_create_dynamic_options);
    }

    static int index_table(lua_State* L)
    {
        options* opts = lua_utility::get_object<options>(L, -2);
        if (!opts)
            return -1;
        int len = 0;
        std::string str;
        if (!lua_utility::try_get_string(L, -1, &str))
            return -1;
        option* opt = opts->get_option(str.c_str());
        if (opt)
        {
            boost::intrusive_ptr<const variant> var = opt->get_value();
            lua_utility::push_variant(L, var.get());
            return 1;
        }
        options* subOpts = opts->get_options(str.c_str());
        if (!subOpts)
        {
            lua_pushnil(L);
            return 1;
        }
        create_wrapper(L, subOpts);
        return 1;
    }
    static options* get_self(lua_State* L, int idx = 1)
    {
        options* self = lua_utility::get_object<options>(L, 1);
        if (self)
            return self;
        luaL_error(L, "xul.options invalid self");
        return NULL;
    }
    static int create(lua_State* L)
    {
        options* opts = lua_utility::get_object<options>(L, -1);
        if (!opts)
            return -1;
        create_wrapper(L, opts);
        return 1;
    }
    static void create_wrapper(lua_State* L, options* opts)
    {
        lua_utility::register_object(L, opts, "xul.options_table");
    }
    static int update(lua_State* L)
    {
        options* self = get_self(L);
        const string_table* params = lua_utility::get_object<string_table>(L, 2);
        if (!params)
            return luaL_error(L, "xul.options invalid params");
        options_utils::update(self, params);
        return 0;
    }
    static int set(lua_State* L)
    {
        options* self = get_self(L);
        std::string key, val;
        if (!lua_utility::try_get_string(L, 2, &key) || !lua_utility::try_get_string(L, 3, &val))
            return -1;
        bool ret = self->set(key.c_str(), val.c_str());
        lua_utility::push_bool(L, ret);
        return 1;
    }
    static int dump(lua_State* L)
    {
        options* self = get_self(L);
        structured_writer* writer = lua_utility::get_object<structured_writer>(L, 2);
        if (!writer)
            return luaL_error(L, "xul.options invalid writer");
        int level = lua_utility::get_integer(L, 3, dump_level::details);
        self->dump(writer, level);
        return 0;
    }
};


}
