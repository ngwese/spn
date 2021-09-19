#pragma once

#include "SPNTouch.h"

#include "lua.h"

typedef struct {
    SPNTouch *touch;
    bool is_owner;
} spn_touch_t;

int spn_touch_open(lua_State *L);
int spn_touch_new(lua_State *L, SPNTouch *t, bool is_owner);
spn_touch_t *spn_touch_check(lua_State *L, int arg);

