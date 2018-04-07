#pragma once

#include <xul/log/log.hpp>
#include <xul/lua/lua_utility.hpp>


namespace xul {


class lua_logger
{
public:
    static const luaL_Reg* get_methods()
    {
        static const luaL_Reg the_methods[] =
        {
            { "new", &lua_logger::create },
            { "debug", &lua_logger::log_debug },
            { "info", &lua_logger::log_info },
            { "warn", &lua_logger::log_warn },
            { "event", &lua_logger::log_event },
            { "error", &lua_logger::log_error },
            { "fatal", &lua_logger::log_fatal },
            { NULL, NULL },
        };
        return the_methods;
    }
    static void register_class(lua_State* L)
    {
        lua_utility::register_object_class(L, "xul.logger", get_methods());
        //lua_utility::register_function(L, "xul", "create_log_filter", &create_filter);
    }

    static void create_filter(lua_State* L)
    {
        std::string name = lua_utility::get_string(L, -1);
        //log_filter* filter = create_log_filter(name.c_str());
    }

    static int create(lua_State* L)
    {
        std::string name = xul::lua_utility::get_string(L, 1);
        if (name.empty())
        {
            lua_pushnil(L);
            return 1;
        }
        xul::logger* lg = xul::logging::get_logger(name.c_str());
        if (!lg)
        {
            lua_pushnil(L);
            return 1;
        }
        lua_utility::register_object(L, lg, "xul.logger");
        return 1;
    }
    static int do_log(lua_State* L, int level)
    {
        //lua_utility::dump_stack(L, "do_log");
        xul::logger* lg = xul::lua_utility::get_object<xul::logger>(L, 1);
        if (!lg || !lg->need_log(level))
            return 0;
#if 0
        lua_Debug dbg;
        lua_getstack(L, 2, &dbg);
        int i = 1;
        const void* self = NULL;
        for (;;)
        {
            const char* varname = lua_getlocal(L, &dbg, i);
            if (varname)
            {
                std::string varnamestr = varname;
                if (varnamestr == "self" && lua_istable(L, -1))
                {
                    self = lua_topointer(L, -1);
                }
                printf("local %d %s %p\n", i, varname, self);
            }
            else
            {
                break;
            }
            ++i;
        }
#endif
        const void* self = lua_topointer(L, 2);
        std::ostringstream temposs;
        int argtop = lua_gettop(L);
        for (int i = 3; i <= argtop; ++i)
        {
            int argtype = lua_type(L, i);
            switch (argtype)
            {
            case LUA_TUSERDATA:
                {
                    temposs << "userdata:" << xul::strings::format("0x%p", lua_touserdata(L, i)) << " ";
                }
                break;
            case LUA_TLIGHTUSERDATA:
                {
                    temposs << "lightuserdata:" << xul::strings::format("0x%p", lua_touserdata(L, i)) << " ";
                }
                break;
            case LUA_TTABLE:
                {
                    const void* argval = lua_topointer(L, i);
                    temposs << "table:" << xul::strings::format("0x%p", argval) << " ";
                }
                break;
            case LUA_TFUNCTION:
                {
                    if (lua_iscfunction(L, i))
                    {
                        lua_CFunction argval = lua_tocfunction(L, i);
                        temposs << "cfunction:" << xul::strings::format("0x%p", argval) << " ";
                    }
                    else
                    {
                        const void* argval = lua_topointer(L, i);
                        temposs << "lfunction:" << xul::strings::format("0x%p", argval) << " ";
                    }
                }
                break;
            case LUA_TBOOLEAN:
                {
                    int argval = lua_toboolean(L, i);
                    temposs << (argval ? "true" : "false") << " ";
                }
                break;
            case LUA_TNUMBER:
                {
                    double argval = lua_tonumber(L, i);
                    temposs << std::setprecision(15) << argval << " ";
                }
                break;
            case LUA_TTHREAD:
                {
                    const void* argval = lua_topointer(L, i);
                    temposs << "thread:" << xul::strings::format("0x%p", argval) << " ";
                }
                break;
            case LUA_TNIL:
                {
                    temposs << "nil ";
                }
                break;
            case LUA_TSTRING:
                {
                    const char* argstr = lua_tostring(L, i);
                    temposs << (argstr ? argstr : "(nil)") << " ";
                }
                break;
            default:
                {
                    const char* argstr = lua_tostring(L, i);
                    temposs << "type:" << argtype << ":" << (argstr ? argstr : " ") << " ";
                }
                break;
            }
        }
        std::string msgstr = temposs.str();
        lg->log(self, level, msgstr.c_str(), msgstr.size());
        //std::string msg = xul::lua_utility::get_string(L, -1);
        //XUL_REL_LOG(lg, lg, level, msg.c_str());
        return 0;
    }
    static int log_debug(lua_State* L)
    {
#if defined(_DEBUG) || !defined(NDEBUG)
        return do_log(L, xul::LOG_DEBUG);
#else
        return 0;
#endif
    }
    static int log_info(lua_State* L)
    {
        return do_log(L, xul::LOG_INFO);
    }
    static int log_warn(lua_State* L)
    {
        return do_log(L, xul::LOG_WARNING);
    }
    static int log_event(lua_State* L)
    {
        return do_log(L, xul::LOG_EVENT);
    }
    static int log_error(lua_State* L)
    {
        return do_log(L, xul::LOG_ERROR);
    }
    static int log_fatal(lua_State* L)
    {
        return do_log(L, xul::LOG_FATAL);
    }
};


}
