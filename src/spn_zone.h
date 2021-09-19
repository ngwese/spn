#pragma once

#include "Zone.h"

#include "lua.h"
#include "lauxlib.h"

int spn_zone_open(lua_State *L);
int spn_zone_new(lua_State *L, soundplane::Zone *z);
soundplane::Zone *spn_zone_check(lua_State *L, int arg);
