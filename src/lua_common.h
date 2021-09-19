#pragma once

#include "lua.h"
#include "lauxlib.h"

int register_object_module(lua_State *L, const char *module_name, const char *class_name, const luaL_Reg *methods, const luaL_Reg *functions);

