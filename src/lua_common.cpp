#include "lua_common.h"

int register_object_module(lua_State *L, const char *module_name, const char *class_name, const luaL_Reg *methods, const luaL_Reg *functions) {
    // module name
    lua_pushstring(L, module_name);

    // define class
    const luaL_Reg *method = methods;

    luaL_newmetatable(L, class_name);      // core, mn, mt
    lua_pushstring(L, "__index");          // core, mn, mt, __index

    lua_newtable(L);                       // core, mn, mt, __index, {}
    lua_pushstring(L, "class");            // core, mn, mt, __index, {}, "class"
    lua_pushstring(L, class_name);         // core, mn, mt, __index, {}, "class", cn
    lua_rawset(L, -3);                     // core, mn ,mt, __index, {}

    for (; method->name; method++) {
        lua_pushstring(L, method->name);
        lua_pushcfunction(L, method->func);
        // methods starting with _ are placed in the metatable, else __index table
        lua_rawset(L, method->name[0] == '_' ? -5 : -3);
    }

    // core, mn, mt, __index, {}
    lua_rawset(L, -3);                    // core, mn, mt

    luaL_setfuncs(L, functions, 0);

    // register the module
    lua_rawset(L, -3);

    return 0;
}
