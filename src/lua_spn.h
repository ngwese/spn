#ifndef __LUA_SPN_H__
#define __LUA_SPN_H__

#define LUA_SPN_VERSION "0.1"

#ifndef LUA_SPN_API
#define LUA_SPN_API __attribute__((visibility("default")))
#endif

extern "C" {

#include "lauxlib.h"
#include "lua.h"

LUA_SPN_API int luaopen_spn(lua_State *L);

} // extern "C"

#endif