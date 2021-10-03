#include "spn_zone.h"
#include "lua_common.h"

//
// prototypes
//

static int spn_zone_free(lua_State *L);
static int spn_zone_tostring(lua_State *L);
static int spn_zone_equals(lua_State *L);
static int spn_zone_name(lua_State *L);
static int spn_zone_base_note(lua_State *L);
static int spn_zone_bounds(lua_State *L);
static int spn_zone_type(lua_State *L);
static int spn_zone_control_numbers(lua_State *L);

static int spn_zone_build(lua_State *L);

//
// touch object methods
//
static const char *spn_zone_modulename = "zone";
static const char *spn_zone_classname = "zone.Zone";

// clang-format off
static luaL_Reg spn_zone_methods[] = {
    {"__gc", spn_zone_free},
    {"__tostring", spn_zone_tostring},
    {"__eq", spn_zone_equals},
    {"name", spn_zone_name},
    {"base_note", spn_zone_base_note},
    {"bounds", spn_zone_bounds},
    {"type", spn_zone_type},
    {"control_numbers", spn_zone_control_numbers},
    {NULL, NULL}
};

static luaL_Reg spn_zone_functions[] = {
    {"new", spn_zone_build},
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

int spn_zone_new(lua_State *L, soundplane::ZoneSpec *spec, bool is_owner) {
    spn_zone_t *ud = (spn_zone_t *)lua_newuserdata(L, sizeof(spn_zone_t));
    ud->spec = spec;
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
        delete z->spec;
    }
}

static int spn_zone_tostring(lua_State *L) {
    static char buffer[256];
    spn_zone_t *z = spn_zone_check(L, 1);
    const char *name = z->spec->getName().getText();
    // TODO: print out something more informative?
    sprintf(buffer, "%s: 0x%p (%s)", spn_zone_classname, z, name);
    lua_pushstring(L, buffer);
    return 1;
}

static int spn_zone_equals(lua_State *L) {
    spn_zone_t *a = spn_zone_check(L, 2);
    spn_zone_t *b = spn_zone_check(L, 1);
    lua_pushboolean(L, a->spec == b->spec);
    return 1;
}

static int spn_zone_name(lua_State *L) {
    spn_zone_t *z = spn_zone_check(L, 1);
    lua_pushstring(L, z->spec->getName().getText());
    return 1;
}

static int spn_zone_base_note(lua_State *L) {
    spn_zone_t *z = spn_zone_check(L, 1);
    lua_pushinteger(L, z->spec->getStartNote());
    return 1;
}

static int spn_zone_bounds(lua_State *L) {
    spn_zone_t *z = spn_zone_check(L, 1);
    const MLRect &bounds = z->spec->getBounds();
    lua_newtable(L);
    // x1
    lua_pushnumber(L, bounds.left());
    lua_seti(L, -2, 1);
    // y1
    lua_pushnumber(L, bounds.top());
    lua_seti(L, -2, 2);
    // width
    lua_pushnumber(L, bounds.width());
    lua_seti(L, -2, 3);
    // height
    lua_pushnumber(L, bounds.height());
    lua_seti(L, -2, 4);
    return 1;
}

static int spn_zone_type(lua_State *L) {
    spn_zone_t *z = spn_zone_check(L, 1);
    lua_pushstring(L, z->spec->getType().getUTF8Ptr());
    return 1;
}

static int spn_zone_control_numbers(lua_State *L) {
    spn_zone_t *z = spn_zone_check(L, 1);
    lua_newtable(L);
    lua_pushnumber(L, z->spec->getController1());
    lua_seti(L, -2, 1);
    // y1
    lua_pushnumber(L, z->spec->getController2());
    lua_seti(L, -2, 2);
    // width
    lua_pushnumber(L, z->spec->getController3());
    lua_seti(L, -2, 3);
    return 1;
}

//
// {
//   name = "something",
//   type = "note_row",
//   bounds = [int, int, width, height],
//   start_note = int,
// }

static int spn_zone_build(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    // bounds
    lua_getfield(L, 1, "bounds");
    luaL_checktype(L, -1, LUA_TTABLE);

    // x1
    lua_geti(L, -1, 1);
    luaL_checktype(L, -1, LUA_TNUMBER);
    uint8_t x1 = lua_tointeger(L, -1);
    lua_pop(L, 1);

    // y1
    lua_geti(L, -1, 2);
    luaL_checktype(L, -1, LUA_TNUMBER);
    uint8_t y1 = lua_tointeger(L, -1);
    lua_pop(L, 1);

    // width
    lua_geti(L, -1, 3);
    luaL_checktype(L, -1, LUA_TNUMBER);
    uint8_t width = lua_tointeger(L, -1);
    lua_pop(L, 1);

    // height
    lua_geti(L, -1, 4);
    luaL_checktype(L, -1, LUA_TNUMBER);
    uint8_t height = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1); // bounds table

    // name
    lua_getfield(L, 1, "name");
    luaL_checktype(L, -1, LUA_TSTRING);
    const char *zone_name = lua_tostring(L, -1);
    // don't pop so name will remain valid

    // type
    lua_getfield(L, 1, "type");
    luaL_checktype(L, -1, LUA_TSTRING);
    const char *zone_type = lua_tostring(L, -1);

    if (strcmp(zone_type, "note_row") == 0) {
        // start_note (optional)
        lua_getfield(L, 1, "start_note");
        uint8_t start_note = 60;
        if (lua_isinteger(L, -1)) {
            start_note = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        // FIXME: this is ugly, first build the zone (copy 1) so that zone_name
        // remains valid
        auto spec = new soundplane::ZoneSpec();
        *spec = soundplane::ZoneSpec::buildNoteRow(zone_name, x1, y1, width,
                                                   height, start_note);
        // pop off type, name, and table arg
        lua_pop(L, 3);
        spn_zone_new(L, spec, true /* is_owner */);
        return 1;

    } else if (strcmp(zone_type, "toggle") == 0) {
        uint8_t ctrl1 = 0;
        lua_getfield(L, 1, "ctrl1");
        if (lua_isinteger(L, -1)) {
            ctrl1 = lua_tointeger(L, -1);
        } else {
            return luaL_error(L, "expected 'ctrl1' to be an integer");
        }
        lua_pop(L, 1);

        auto spec = new soundplane::ZoneSpec();
        *spec = soundplane::ZoneSpec::buildToggle(zone_name, x1, y1, width,
                                                  height, ctrl1);
        // pop off type, name, and table arg
        lua_pop(L, 3);
        // push on new user data (copy 2)
        spn_zone_new(L, spec, true /* is_owner */);
        return 1;

    } else if (strcmp(zone_type, "xy") == 0) {
        uint8_t ctrl1 = 0;
        uint8_t ctrl2 = 0;

        lua_getfield(L, 1, "ctrl1");
        if (lua_isinteger(L, -1)) {
            ctrl1 = lua_tointeger(L, -1);
        } else {
            return luaL_error(L, "expected 'ctrl1' to be an integer");
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "ctrl2");
        if (lua_isinteger(L, -1)) {
            ctrl2 = lua_tointeger(L, -1);
        } else {
            return luaL_error(L, "expected 'ctrl2' to be an integer");
        }
        lua_pop(L, 1);

        auto spec = new soundplane::ZoneSpec();
        *spec = soundplane::ZoneSpec::buildXY(zone_name, x1, y1, width, height,
                                              ctrl1, ctrl2);
        // pop off type, name, and table arg
        lua_pop(L, 3);
        spn_zone_new(L, spec, true /* is_owner */);
        return 1;

    } else if (strcmp(zone_type, "x") == 0) {
        uint8_t ctrl1 = 0;

        lua_getfield(L, 1, "ctrl1");
        if (lua_isinteger(L, -1)) {
            ctrl1 = lua_tointeger(L, -1);
        } else {
            return luaL_error(L, "expected 'ctrl1' to be an integer");
        }
        lua_pop(L, 1);

        auto spec = new soundplane::ZoneSpec();
        *spec = soundplane::ZoneSpec::buildX(zone_name, x1, y1, width, height,
                                             ctrl1);
        // pop off type, name, and table arg
        lua_pop(L, 3);
        spn_zone_new(L, spec, true /* is_owner */);
        return 1;
    } else if (strcmp(zone_type, "y") == 0) {
        uint8_t ctrl1 = 0;

        lua_getfield(L, 1, "ctrl1");
        if (lua_isinteger(L, -1)) {
            ctrl1 = lua_tointeger(L, -1);
        } else {
            return luaL_error(L, "expected 'ctrl1' to be an integer");
        }
        lua_pop(L, 1);

        auto spec = new soundplane::ZoneSpec();
        *spec = soundplane::ZoneSpec::buildY(zone_name, x1, y1, width, height,
                                             ctrl1);
        // pop off type, name, and table arg
        lua_pop(L, 3);
        spn_zone_new(L, spec, true /* is_owner */);
        return 1;
    }

    // pop off type, name, and table arg
    lua_pop(L, 3);
    // signal an error
    return luaL_error(L, "unsupported `zone_type`");
}
