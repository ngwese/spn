#include "spn_touch.h"
#include "lua_common.h"

#include "SPNTouchPool.h"

//
// prototypes
//

static int spn_touch_free(lua_State *L);
static int spn_touch_tostring(lua_State *L);
static int spn_touch_equals(lua_State *L);

//
// touch object methods
//
static const char *spn_touch_modulename = "spn";
static const char *spn_touch_classname = "spn.Touch";

// clang-format off
static luaL_Reg spn_touch_methods[] = {
    {"__gc", spn_touch_free},
    {"__tostring", spn_touch_tostring},
    {"__eq", spn_touch_equals},
    {NULL, NULL}
};

static luaL_Reg spn_touch_functions[] = {
    {NULL, NULL}
};
// clang-format on

//
// module
//

int spn_touch_open(lua_State *L) {
    return register_object_module(L, spn_touch_modulename, spn_touch_classname,
                                  spn_touch_methods, spn_touch_functions);
}

int spn_touch_new(lua_State *L, SPNTouch *t, bool is_owner) {
    spn_touch_t *ud = (spn_touch_t *)lua_newuserdata(L, sizeof(spn_touch_t));
    ud->touch = t;
    ud->is_owner = is_owner;
    luaL_getmetatable(L, spn_touch_classname);
    lua_setmetatable(L, -2);
    return 1;
}

spn_touch_t *spn_touch_check(lua_State *L, int arg) {
    void *ud = luaL_checkudata(L, arg, spn_touch_classname);
    luaL_argcheck(L, ud != NULL, arg, "'Touch' expected");
    return (spn_touch_t *)ud;
}

static int spn_touch_free(lua_State *L) {
    spn_touch_t *t = spn_touch_check(L, 1);
    if (t->is_owner) {
        SPNTouchPool::destroy(t->touch);
    }
}

static int spn_touch_tostring(lua_State *L) {
    char buffer[128];
    spn_touch_t *t = spn_touch_check(L, 1);
    // TODO: print out something more informative?
    sprintf(buffer, "%s: 0x%p", spn_touch_classname, t);
    lua_pushstring(L, buffer);
    return 1;
}

static int spn_touch_equals(lua_State *L) {
    spn_touch_t *a = spn_touch_check(L, 2);
    spn_touch_t *b = spn_touch_check(L, 1);
    lua_pushboolean(L, a->touch == b->touch);
    return 1;
}
