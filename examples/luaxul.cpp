#include <xul/lua/lua.hpp>
#include <xul/lua/lua_bindings.hpp>
#include <xul/log/log_manager.hpp>
#include <stdio.h>

extern "C" int XUL_EXPORT luaopen_xul(lua_State* L)
{
    xul::register_lua(L);
    //? should make sure the table for 'xul' is put on the stack
    lua_getglobal(L, "xul");
    return 1;
}
