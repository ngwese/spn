#include "spn_zone.h"
#include "lua_common.h"

//
// prototypes
//

static int spn_zone_free(lua_State *L);
static int spn_zone_tostring(lua_State *L);
static int spn_zone_equals(lua_State *L);

//
// touch object methods
//
static const char *spn_zone_modulename = "spn";
static const char *spn_zone_classname = "spn.Zone";

// clang-format off
static luaL_Reg spn_zone_methods[] = {
    {"__gc", spn_zone_free},
    {"__tostring", spn_zone_tostring},
    {"__eq", spn_zone_equals},
    {NULL, NULL}
};

static luaL_Reg spn_zone_functions[] = {
    {NULL, NULL}
};
// clang-format on

//
// module
//

int spn_zone_open(lua_State *L) {
    return register_object_module(L, spn_zone_modulename, spn_zone_classname,
                                  spn_zone_methods, spn_zone_functions);
}

int spn_zone_new(lua_State *L, soundplane::Zone *z, bool is_owner) {
    spn_zone_t *ud = (spn_zone_t *)lua_newuserdata(L, sizeof(spn_zone_t));
    ud->zone = z;
    ud->is_owner = is_owner;
    luaL_getmetatable(L, spn_zone_classname);
    lua_setmetatable(L, -2);
    return 1;
}

spn_zone_t *spn_zone_check(lua_State *L, int arg) {
    void *ud = luaL_checkudata(L, arg, spn_zone_classname);
    luaL_argcheck(L, ud != NULL, arg, "'Zone' expected");
    return (spn_zone_t *)ud;
}

static int spn_zone_free(lua_State *L) {
    spn_zone_t *z = spn_zone_check(L, 1);
    if (z->is_owner) {
        delete z->zone;
    }
}

static int spn_zone_tostring(lua_State *L) {
    char buffer[128];
    spn_zone_t *t = spn_zone_check(L, 1);
    // TODO: print out something more informative?
    sprintf(buffer, "%s: 0x%p", spn_zone_classname, t);
    lua_pushstring(L, buffer);
    return 1;
}

static int spn_zone_equals(lua_State *L) {
    spn_zone_t *a = spn_zone_check(L, 2);
    spn_zone_t *b = spn_zone_check(L, 1);
    lua_pushboolean(L, a->zone == b->zone);
    return 1;
}
