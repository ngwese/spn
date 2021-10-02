#pragma once

#include "ZoneSpec.h"

#include "lua.h"

typedef struct {
    soundplane::ZoneSpec *spec;
    bool is_owner;
} spn_zone_t;

int spn_zone_open(lua_State *L);
int spn_zone_new(lua_State *L, soundplane::ZoneSpec *spec, bool is_owner);
spn_zone_t *spn_zone_check(lua_State *L, int arg);
