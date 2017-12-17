#include <xul/lua/lua.hpp>
#include <xul/lua/lua_bindings.hpp>
#include <xul/log/log_manager.hpp>
#include <stdio.h>

int main(int argc, char** argv)
{
    xul::log_manager::start_console_log_service("xlua");
    if (argc < 2)
    {
        printf("no argument\n");
        printf("USAGE: xlua SCRIPT_FILE\n");
        return 1;
    }
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        printf("cannot create state\n");
        return 1;
    }
    luaL_openlibs(L);
    xul::register_lua(L);
    int ret = luaL_dofile(L, argv[1]);
    printf("luaL_dofile return:%d\n", ret);
    if (ret != 0)
    {
        xul::lua_utility::handle_error(L);
    }
    lua_close(L);
    return ret;
}
