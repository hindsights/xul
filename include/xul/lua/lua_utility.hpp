#pragma once

#include <xul/lua/lua.hpp>
#include <xul/data/variant.hpp>
#include <xul/log/log.hpp>
#include <xul/lua/lua_stack.hpp>
#include <xul/macro/memory.hpp>
#include <xul/macro/iterate.hpp>
#include <iomanip>

namespace xul {


class lua_utility
{
public:
    static const char *luaL_findtable (lua_State *L, int idx,
                                       const char *fname, int szhint) {
      const char *e;
      if (idx) lua_pushvalue(L, idx);
      do {
        e = strchr(fname, '.');
        if (e == NULL) e = fname + strlen(fname);
        lua_pushlstring(L, fname, e - fname);
        if (lua_rawget(L, -2) == LUA_TNIL) {  /* no such field? */
          lua_pop(L, 1);  /* remove this nil */
          lua_createtable(L, 0, (*e == '.' ? 1 : szhint)); /* new table for field */
          lua_pushlstring(L, fname, e - fname);
          lua_pushvalue(L, -2);
          lua_settable(L, -4);  /* set new table into field */
        }
        else if (!lua_istable(L, -1)) {  /* field has a non-table value? */
          lua_pop(L, 2);  /* remove table and value */
          return fname;  /* return problematic part of the name */
        }
        lua_remove(L, -2);  /* remove previous table */
        fname = e + 1;
      } while (*e == '.');
      return NULL;
    }
    
    static void luaL_pushmodule (lua_State *L, const char *modname,
                                     int sizehint) {
      luaL_findtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE, 1);
      if (lua_getfield(L, -1, modname) != LUA_TTABLE) {  /* no LOADED[modname]? */
        lua_pop(L, 1);  /* remove previous result */
        /* try global variable (and create one if it does not exist) */
        lua_pushglobaltable(L);
        if (luaL_findtable(L, 0, modname, sizehint) != NULL)
          luaL_error(L, "name conflict for module '%s'", modname);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, modname);  /* LOADED[modname] = new table */
      }
      lua_remove(L, -2);  /* remove LOADED table */
    }
    
    static int libsize (const luaL_Reg *l) {
      int size = 0;
      for (; l && l->name; l++) size++;
      return size;
    }
    static void luaL_openlib (lua_State *L, const char *libname,
            const luaL_Reg *l, int nup) {
        luaL_checkversion(L);
        if (libname) {
            luaL_pushmodule(L, libname, libsize(l));  /* get/create library table */
            lua_insert(L, -(nup + 1));  /* move library table to below upvalues */
        }
        if (l)
            luaL_setfuncs(L, l, nup);
        else
            lua_pop(L, nup);  /* remove upvalues */
    }
    static void luaL_register (lua_State *L, const char *libname, const luaL_Reg *l) {
        luaL_openlib(L, libname, l, 0);
    }
    #define LUA_GLOBALSINDEX    LUA_RIDX_GLOBALS
    
    static int handle_error(lua_State* L)
    {
        //dump_stack(L, "handle_error");
        const char* errmsg = lua_tostring(L, -1);
        XUL_APP_REL_ERROR("handle_error " << (errmsg ? errmsg : ""));
        dump_call_stack(L, "handle_error");
#if defined(_DEBUG) || !defined(NDEBUG)
        XUL_APP_REL_ERROR("handle_error exit");
        ::exit(100);
#endif
        return 0;
    }
public:
    static int pcall(lua_State* L, int argcount)
    {
#if (defined(_DEBUG) || !defined(NDEBUG)) && 0
        int errfunc = -argcount - 2;
        lua_pushcfunction(L, &lua_utility::handle_error);
        lua_insert(L, errfunc);
        int ret = lua_pcall(L, argcount, LUA_MULTRET, errfunc);
        return check_call_result(L, ret);
#else
        int ret = lua_pcall(L, argcount, LUA_MULTRET, 0);
        return check_call_result(L, ret);
#endif
    }
    static int check_call_result(lua_State* L, int ret)
    {
        if (ret != 0)
        {
            const char* errmsg = lua_tostring(L, -1);
            if (errmsg)
            {
                XUL_APP_REL_ERROR("check_call_result " << ret << " " << (errmsg ? errmsg : ""));
                //lua_pop(L, 1);
                dump_call_stack(L, "pcall");
            }
            return ret;
        }
        return ret;
    }
    static void dump_table(lua_State* L, int idx, const char* tag)
    {
        XUL_APP_DEBUG("dump_table: begin " << idx << " " << tag);
        assert(lua_istable(L, idx));
#if (defined(_DEBUG) || !defined(NDEBUG)) && 0
        lua_pushnil(L);
        while( 0 != lua_next( L, idx - 1 ) )
        {
            const char* key = lua_tostring(L, -2);
            int vtype = lua_type(L, -1);
            lua_pop( L, 1 );
            XUL_APP_DEBUG("dump_table: " << key << " " << vtype);
        }
#endif
    }
    static void dump_call_stack(lua_State *L, const char* tag)
    {
        XUL_APP_DEBUG("dump_call_stack begin " << tag);
#if 0
        lua_Debug dbginfo;
        for (int i = 2;; ++i)
        {
            int ret = lua_getstack(L, i, &dbginfo);
            if (0 == ret)
            {
                XUL_APP_DEBUG("dump_call_stack end " << tag << " at level " << i << " " << ret);
                break;
            }
            XUL_APP_DEBUG("dump_call_stack level= " << i
                << " name=" << dbginfo.name
                << " source=" << dbginfo.source << ":" << dbginfo.currentline << ":" << xul::make_tuple(dbginfo.linedefined, dbginfo.lastlinedefined)
                << " namewhat=" << dbginfo.namewhat
                << " what=" << dbginfo.what
                << " short_src=" << dbginfo.short_src);
        }
#else
        lua_auto_stack_cleaner stack_cleaner(L);
        lua_getglobal(L, "Stack");
        if (!lua_isnil(L, -1))
        {
            lua_getfield(L, -1, "dump");
            if (!lua_isnil(L, -1))
            {
                pcall(L, 0);
            }
        }
#endif
    }
    static void dump_stack(lua_State *L, const char* tag)
    {
#if (defined(_DEBUG) || !defined(NDEBUG)) && 0
        XUL_APP_DEBUG("dump_stack: begin " << L << ":" << tag << " " << lua_gettop(L));
        //printf("dump_stack: begin %s %d\n", tag, lua_gettop(L));
        int i;
        int top = lua_gettop(L);
        for (i = 1; i <= top; i++) {
            int t = lua_type(L, i);
            switch (t) {

            case LUA_TSTRING:
                XUL_APP_DEBUG("dump_stack: string " << lua_tostring(L, i));
                //printf("dump_stack: string %s\n", lua_tostring(L, i));
                break;

            case LUA_TBOOLEAN:
                XUL_APP_DEBUG("dump_stack: bool " << lua_toboolean(L, i) ? "true" : "false");
                //printf("dump_stack: bool %s\n", lua_toboolean(L, i) ? "true" : "false");
                break;

            case LUA_TNUMBER:
                XUL_APP_DEBUG("dump_stack: number " << std::setprecision(15) << lua_tonumber(L, i));
                //printf("dump_stack: number %f\n", lua_tonumber(L, i));
                break;

            case LUA_TLIGHTUSERDATA:
                XUL_APP_DEBUG("dump_stack: lightuserdata " << lua_touserdata(L, i));
                //printf("dump_stack: number %f\n", lua_tonumber(L, i));
                break;
            case LUA_TUSERDATA:
                XUL_APP_DEBUG("dump_stack: userdata " << lua_touserdata(L, i));
                //printf("dump_stack: number %f\n", lua_tonumber(L, i));
                break;
            case LUA_TTABLE:
                XUL_APP_DEBUG("dump_stack: table " << lua_topointer(L, i));
                //printf("dump_stack: number %f\n", lua_tonumber(L, i));
                break;
            case LUA_TFUNCTION:
                XUL_APP_DEBUG("dump_stack: function " << lua_topointer(L, i));
                //printf("dump_stack: number %f\n", lua_tonumber(L, i));
                break;

            default:
                XUL_APP_DEBUG("dump_stack: typename " << lua_typename(L, t));
                //printf("dump_stack: typename %s\n", lua_typename(L, t));
                break;

            }
        }
#endif
    }

    static void register_methods_light_userdata(lua_State* L, luaL_Reg* methods)
    {
        lua_pushlightuserdata(L, methods);
        lua_gettable(L, LUA_REGISTRYINDEX);
        if (lua_isnil(L, -1)) {
            /* not found */
            lua_pop(L, 1);

            lua_newtable(L);
            luaL_register(L, NULL, methods);

            lua_pushlightuserdata(L, methods);
            lua_pushvalue(L, -2);
            lua_settable(L, LUA_REGISTRYINDEX);
        }
    }
    static bool save_methods_table(lua_State* L, const char* name, const luaL_Reg* methods)
    {
        assert(lua_istable(L, -1));
        lua_pushstring(L, name);
        create_methods_table(L, methods);
        lua_settable(L, -3);
        return true;
    }
    static void create_methods_table(lua_State* L, const luaL_Reg* methods)
    {
        //luaL_register(L, "rtmfp.NetConnection", rtmfp_NetConnection_metatable);
        lua_newtable(L);
        while (methods->name != NULL)
        {
            lua_pushstring(L, methods->name);
            lua_pushcfunction(L, methods->func);
            dump_stack(L, "create_methods_table");
            lua_settable(L, -3);
            XUL_APP_DEBUG("register method " << methods->name << " " << methods->func);
            ++methods;
        }
        dump_stack(L, "create_methods_table");
    }

    static void register_metatable(lua_State* L, const luaL_Reg* metatable)
    {
        //luaL_register(L, "rtmfp.NetConnection", rtmfp_NetConnection_metatable);
        lua_newtable(L);
        while (metatable->name != NULL)
        {
            lua_pushstring(L, metatable->name);
            lua_pushcfunction(L, metatable->func);
            dump_stack(L, "register_metatable");
            lua_settable(L, -3);
            XUL_APP_DEBUG("register metatable item " << metatable->name << " " << metatable->func);
            ++metatable;
        }
        dump_stack(L, "register_metatable");
        lua_setmetatable(L, -2);
    }
    static void register_class(lua_State* L, const char* name, const luaL_Reg* metatable, const luaL_Reg* methods)
    {
        XUL_APP_DEBUG("register_class " << name);
        dump_stack(L, "before register");
        luaL_register(L, name, methods); // push a table
        //lua_utility::dump_stack(L);
        luaL_newmetatable(L, name); // push a table
        //lua_utility::dump_stack(L);
        luaL_register(L, NULL, metatable); // modify table
        //lua_utility::dump_stack(L);
        lua_pushliteral(L, "__index");
        //lua_utility::dump_stack(L);
        lua_pushvalue(L, -3);
        //lua_utility::dump_stack(L);
        lua_rawset(L, -3);
        lua_pop(L, 2);
        dump_stack(L, "after register");
        //lua_utility::dump_stack(L);
    }
    static void register_table_class(lua_State* L, const char* name, const luaL_Reg* metatable, const luaL_Reg* methods)
    {
        XUL_APP_DEBUG("register_table_class " << name);
        dump_stack(L, "before register_table_class");
        luaL_register(L, name, methods);
        luaL_newmetatable(L, name);
        luaL_register(L, NULL, metatable);
        dump_stack(L, "after register_table_class");
        lua_pop(L, 2);
    }
    template <typename CObjectT>
    static int destroy_cobject(lua_State* L)
    {
        int type = lua_type(L, -1);
        if (type != LUA_TUSERDATA)
            return -1;
        CObjectT** pobj = static_cast<CObjectT**>(lua_touserdata(L, -1));
        if (pobj && *pobj)
        {
            cobject_release_ref(*pobj);
            return 0;
        }
        return -1;
    }
    template <typename CObjectT>
    static const luaL_Reg* get_cobject_metatable()
    {
        static const luaL_Reg the_metatable[] =
        {
            { "__gc", &xul::lua_utility::destroy_cobject<CObjectT> },
            { NULL, NULL },
        };
        return the_metatable;
    }
    static const luaL_Reg* get_object_metatable()
    {
        static const luaL_Reg the_metatable[] =
        {
            { "__gc", &xul::lua_utility::destroy_object },
            { NULL, NULL },
        };
        return the_metatable;
    }
    static void register_functions(lua_State* L, const char* module, const luaL_Reg *l)
    {
        XUL_APP_DEBUG("register_functions " << module);
        dump_stack(L, "before register_functions");
        luaL_register(L, module, l);
        dump_stack(L, "after register_functions");
        lua_pop(L, 1);
    }
    static void register_function(lua_State* L, const char* moduleName, const char* funcName, lua_CFunction f)
    {
        XUL_APP_DEBUG("register_function " << moduleName << "." << funcName << " " << f);
        dump_stack(L, "before register_function");
        lua_getglobal(L, moduleName);
        lua_utility::dump_stack(L, "register_function");
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_setglobal(L, moduleName);
        }
        lua_pushcfunction(L, f);
        lua_setfield(L, -2, funcName);
        dump_stack(L, "after register_function");
        lua_pop(L, 1);
    }
    template <typename CObjectT>
    static void register_cobject_class(lua_State* L, const char* name, const luaL_Reg* methods)
    {
        register_class(L, name, get_cobject_metatable<CObjectT>(), methods);
    }
    template <typename CObjectT>
    static void register_cobject(lua_State* L, CObjectT* obj, const char* name)
    {
        cobject_add_ref(obj);
        lua_utility::dump_stack(L, "register_cobject");
        void* p = lua_newuserdata(L, sizeof(CObjectT*));
        XUL_WRITE_MEMORY(p, obj, CObjectT*);
        luaL_getmetatable(L, name);
        lua_setmetatable(L, -2);
        lua_utility::dump_stack(L, "register_cobject");
    }
    static void register_object_class(lua_State* L, const char* name, const luaL_Reg* methods)
    {
        register_class(L, name, get_object_metatable(), methods);
    }
    static void register_object(lua_State* L, object* obj, const char* name)
    {
        obj->add_reference();
        lua_utility::dump_stack(L, "register_object0");
        void* p = lua_newuserdata(L, sizeof(object*));
        XUL_WRITE_MEMORY(p, obj, object*);
        luaL_getmetatable(L, name);
        lua_setmetatable(L, -2);
        lua_utility::dump_stack(L, "register_object1");
    }
    static int destroy_object(lua_State* L)
    {
        int type = lua_type(L, -1);
        if (type != LUA_TUSERDATA)
            return -1;
        object** pobj = static_cast<object**>(lua_touserdata(L, -1));
        if (pobj && *pobj)
        {
            (*pobj)->release_reference();
            return 0;
        }
        return -1;
    }

    template <typename T>
    static T* get_object(lua_State* L, int idx)
    {
        return static_cast<T*>(get_object_pointer(L, idx));
    }

    static void* get_object_pointer(lua_State* L, int idx)
    {
        void* ptr = NULL;
        int type = lua_type(L, idx);
        if (LUA_TLIGHTUSERDATA == type)
            ptr = lua_touserdata(L, idx);
        else if (LUA_TUSERDATA == type)
        {
            void* p = lua_touserdata(L, idx);
            void** pobj = static_cast<void**>(p);
            ptr = pobj ? *pobj : NULL;
        }
        if (NULL == ptr)
        {
            XUL_LIBXUL_REL_WARN("get_object_pointer null " << idx);
        }
        return ptr;
    }

    static void* get_userdata(lua_State* L, int idx, void* defaultVal)
    {
        void* val = 0;
        return try_get_userdata(L, idx, &val) ? val : defaultVal;
    }
    static bool try_get_userdata(lua_State* L, int idx, void** val)
    {
        assert(val);
        if (!lua_isuserdata(L, idx))
            return false;
        *val = lua_touserdata(L, idx);
        return true;
    }
    static bool pop_bool(lua_State* L, bool defaultVal)
    {
        bool ret = get_bool(L, -1, defaultVal);
        lua_pop(L, 1);
        return ret;
    }
    static bool try_get_bool(lua_State* L, int idx, bool* val)
    {
        assert(val);
        if (!lua_isboolean(L, idx))
            return false;
        *val = (lua_toboolean(L, idx) != 0);
        return true;
    }
    static bool get_bool(lua_State* L, int idx, bool defaultVal)
    {
        if (!lua_isboolean(L, idx))
            return defaultVal;
        int val = lua_toboolean(L, idx);
        return val != 0;
    }
    static int64_t get_integer(lua_State* L, int idx, int64_t defaultVal)
    {
        int64_t val = 0;
        return try_get_integer(L, idx, &val) ? val : defaultVal;
    }
    static int64_t get_integer_or_nil(lua_State* L, int idx, int64_t defaultVal)
    {
        if (!lua_isnil(L, idx))
            return defaultVal;
        int64_t val = 0;
        return try_get_integer(L, idx, &val) ? val : defaultVal;
    }
    static bool try_get_integer(lua_State* L, int idx, int64_t* val)
    {
        assert(val);
        if (!lua_isnumber(L, idx))
            return false;
        *val = lua_tonumber(L, idx);
        return true;
    }
    static int get_integer(lua_State* L, int idx, int defaultVal)
    {
        int64_t val = 0;
        return try_get_integer(L, idx, &val) ? val : defaultVal;
    }
    static int get_integer_or_nil(lua_State* L, int idx, int defaultVal)
    {
        if (lua_isnil(L, idx))
            return defaultVal;
        int64_t val = 0;
        return try_get_integer(L, idx, &val) ? val : defaultVal;
    }
    static bool try_get_integer(lua_State* L, int idx, int* val)
    {
        assert(val);
        if (!lua_isnumber(L, idx))
            return false;
        *val = lua_tonumber(L, idx);
        return true;
    }
    static std::string get_string(lua_State* L, int idx)
    {
        std::string s;
        if (try_get_string(L, idx, &s))
            return s;
        return std::string();
    }
    static bool try_get_string(lua_State* L, int idx, std::string* str)
    {
        assert(str);
        size_t len = 0;
        const char* s = lua_tolstring(L, idx, &len);
        if (!s || len <= 0)
        {
            return false;
        }
        str->assign(s, len);
        return true;
    }
    static LUA_NUMBER get_number(lua_State* L, int idx, LUA_NUMBER defaultVal)
    {
        LUA_NUMBER val;
        if (try_get_number(L, idx, &val))
            return val;
        return defaultVal;
    }
    static bool try_get_number(lua_State* L, int idx, LUA_NUMBER* val)
    {
        assert(val);
        if (!lua_isnumber(L, idx))
            return false;
        *val = lua_tonumber(L, idx);
        return true;
    }

    static void push_variant_dict( lua_State* L, const variant::dict_type* items )
    {
        if (items)
        {
            lua_newtable(L);
            XUL_ITERATE(variant::dict_type, items, iter)
            {
                const variant::dict_type::entry_type* entry = iter->element();
                lua_pushstring(L, entry->get_key());
                push_variant(L, entry->get_value());
                dump_stack(L, "push_variant_dict1");
                lua_settable(L, -3);
                dump_stack(L, "push_variant_dict2");
            }
        }
        else
        {
            lua_pushnil(L);
            dump_stack(L, "push_variant_dict3");
        }
    }
    static void push_variant_list( lua_State* L, const variant::list_type* items, int pos = 0 )
    {
        if (items)
        {
            lua_newtable(L);
            for (int i = pos; i < items->size(); ++i)
            {
                lua_pushinteger(L, i - pos + 1);
                push_variant(L, items->get(i));
                lua_settable(L, -3);
                dump_stack(L, "push_variant_list1");
                dump_stack(L, "push_variant_list2");
            }
        }
        else
        {
            lua_pushnil(L);
            dump_stack(L, "push_variant_list3");
        }
    }

    static variant* load_variant( lua_State* L, int idx )
    {
        int type = lua_type(L, idx);
        switch (type)
        {
        case LUA_TNIL:
            return create_null_variant();
        case LUA_TBOOLEAN:
            return create_bool_variant(lua_toboolean(L, idx) != 0);
        case LUA_TNUMBER:
            return create_integer_variant(lua_tointeger(L, idx));
        case LUA_TSTRING:
            return create_string_variant(lua_tostring(L, idx));
        case LUA_TTABLE:
            {
                if (idx < 0)
                {
                    idx = lua_gettop(L) + idx + 1;
                }
                variant* var = create_variant();
                //variant::dict_type* items = var->set_dict();
                lua_pushnil(L);

                dump_stack(L, "load_variant1");
                int refff = lua_next( L, idx);

               // while( 0 != lua_next( L, idx ) )
                if (lua_isnumber(L, -2)==1)
                {
                    variant::list_type* listitems = var->set_list();
                    while (0 != refff)
                    {
                        //XUL_APP_DEBUG("load_variant idx " << idx);
                        dump_stack(L, "after next");

                            int key = lua_tonumber(L, -2);
                            variant* item = load_variant(L, -1);
                            XUL_APP_DEBUG("dump_table: begin " << key);
                            listitems->add(item);
                            //items->set(key, item);

                        dump_stack(L, "load_variant2");
                        lua_pop( L, 1 );
                        dump_stack(L, "before next");
                        refff = lua_next( L, idx);
                        XUL_APP_DEBUG("dump_table: begin " << refff);
                    }
                    dump_stack(L, "load_variant3");
                    return var;
                }
                else
                {
                    variant::dict_type* dictitems = var->set_dict();
                    while (0 != refff)
                    {
                        //XUL_APP_DEBUG("load_variant idx " << idx);
                        dump_stack(L, "after next");

                            const char* key = lua_tostring(L, -2);
                            variant* item = load_variant(L, -1);
                            dictitems->set(key, item);

                        dump_stack(L, "load_variant2");
                        lua_pop( L, 1 );
                        dump_stack(L, "before next");
                        refff = lua_next( L, idx);
                        XUL_APP_DEBUG("dump_table: begin " << refff);
                    }
                    dump_stack(L, "load_variant3");
                    return var;
                }
            }
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
            {
                void* uv = lua_touserdata(L, idx);
                if (NULL == uv)
                {
                    return create_null_variant();
                }
                xul::variant_tagged_buffer* tag = get_object<xul::variant_tagged_buffer>(L, -1);
                variant* var = create_variant();
                var->set_tagged_buffer(tag);
                return var;
            }
            break;
        default:
            assert(false);
            break;
        }
        return NULL;
    }

    static void push_bool(lua_State* L, bool val)
    {
        lua_pushboolean(L, val ? 1 : 0);
    }
    static void push_string(lua_State* L, const std::string& val)
    {
        lua_pushlstring(L, val.c_str(), val.size());
    }
    static void push_string(lua_State* L, const char* val)
    {
        lua_pushstring(L, val);
    }
    static void push_null(lua_State* L)
    {
        lua_getglobal(L, "xul");
        lua_getfield(L, -1, "null");
        lua_remove(L, -2);
    }

    static void push_variant( lua_State* L, const variant* var )
    {
        int vtype = var->get_type();
        switch (vtype)
        {
        case variant_type_null:
            push_null(L);
            break;
        case variant_type_bool:
            lua_pushboolean(L, var->get_bool(false));
            break;
        case variant_type_integer:
            lua_pushinteger(L, var->get_integer(0));
            break;
        case variant_type_float:
            lua_pushnumber(L, var->get_float(0));
            break;
        case variant_type_string:
            {
                int len = 0;
                const char* str = var->get_bytes(&len);
                if (str)
                {
                    lua_pushlstring(L, str, len);
                }
            }
            break;
        case variant_type_pointer:
            lua_pushlightuserdata(L, var->get_pointer());
            break;
        case variant_type_object:
            lua_pushlightuserdata(L, var->get_object());
            break;
        case variant_type_taged_buffer:
            {
                // use tag as class name
                const variant_tagged_buffer* tag = var->get_tagged_buffer();
                assert(tag);
                //lua_getfield(L, LUA_GLOBALSINDEX, tag);
                luaL_findtable(L, LUA_GLOBALSINDEX, tag->get_tag(), 1);
                dump_stack(L, "lua_getglobal");
                if (lua_istable(L, -1))
                {
                    lua_pushstring(L, "new");
                    dump_stack(L, "push new");
                    lua_gettable(L, -2);
                    dump_stack(L, "gettable");
                    if (lua_isfunction(L, -1))
                    {
                        int bufsize = tag->get_size();
                        const uint8_t* buf = tag->get_data();
                        lua_pushlstring(L, reinterpret_cast<const char*>(buf), bufsize);
                        dump_stack(L, "push bytes");
                        lua_pcall(L, 1, 1, 0);
                        lua_remove(L, -2);
                        dump_stack(L, "pcall");
                    }
                    else
                    {
                        dump_stack(L, "pcall");
                        lua_pop(L, 1);
                    }
                }
                else
                {
                    dump_stack(L, "pcall");
                    lua_pop(L, 1);
                }
            }
            break;
        case variant_type_list:
            {
                const variant::list_type* items = var->get_list();
                push_variant_list(L, items);
            }
            break;
        case variant_type_dict:
            {
                const variant::dict_type* items = var->get_dict();
                push_variant_dict(L, items);
            }
            break;
        }
    }

    static void set_table( lua_State* L, const char* key, const char* val, int idx )
    {
        dump_stack(L, "set_table");
        lua_pushstring(L, key);
        lua_pushstring(L, val);
        lua_settable(L, idx - 2);
    }

    static void register_null(lua_State* L)
    {
        lua_getglobal(L, "xul");
        dump_stack(L, "xul");
        assert(lua_istable(L, -1));
        lua_pushlightuserdata(L, NULL);
        lua_setfield(L, -2, "null");
        dump_stack(L, "xul.null");
        lua_pop(L, 1);
    }

};


}
