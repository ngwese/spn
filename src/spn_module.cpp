#include "spn_module.h"

// #include <chrono>
#include <iostream>
// #include <memory>
// #include <thread>

// lua
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

// soundplaneclient
#include "Client.h"
#include "ConsoleOutput.h"
#include "Logging.h"
#include "MLProjectInfo.h"
#include "Touch.h"
#include "Zone.h"

// this
#include "SPNOutput.h"
#include "spn_touch.h"
#include "spn_zone.h"

//
// state
//
static bool sClientRunning = false;
static soundplane::Client *sClient = nullptr;
static ml::SharedResourcePointer<ml::Timers> sTimers;
static SPNOutput sOutput{};
// static soundplane::ConsoleOutput sOutput{1000};

//
// prototypes
//
static int spn_client_start(lua_State *L);
static int spn_client_stop(lua_State *L);
static int spn_client_is_running(lua_State *L);
static int spn_client_verbose(lua_State *L);
static int spn_client_set_property(lua_State *L);
static int spn_client_get_property(lua_State *L);
static int spn_client_zone_preset(lua_State *L);
static int spn_client_set_zones(lua_State *L);
static int spn_client_calibrate(lua_State *L);

//
// module definition
//

// clang-format off
static const luaL_Reg mod[] = {
    {"touch", spn_touch_open},
    {"zone", spn_zone_open},
    {NULL, NULL}
};

static luaL_Reg func[] = {
    {"start", spn_client_start},
    {"stop", spn_client_stop},
    {"is_running", spn_client_is_running},
    {"verbose", spn_client_verbose},
    {"set_property", spn_client_set_property},
    {"get_property", spn_client_get_property},
    {"zone_preset", spn_client_zone_preset},
    {"set_zones", spn_client_set_zones},
    {"calibrate", spn_client_calibrate},
    {NULL, NULL}
};
// clang-format on

LUA_SPN_API int luaopen__spn(lua_State *L) {
    lua_newtable(L);

    // register/open sub-modules
    for (int i = 0; mod[i].name; i++) {
        mod[i].func(L);
    }

    luaL_setfuncs(L, func, 0);

    lua_pushstring(L, "VERSION");
    lua_pushstring(L, LUA_SPN_VERSION);
    lua_rawset(L, -3);

    return 1;
}

//
// implementation
//

static int spn_client_start(lua_State *L) {
    if (!sClientRunning) {
        soundplane::Console()
            << "spn: starting client v" << MLProjectInfo::versionString << "\n";

        sTimers->start(true /* on main thread */);

        sClient = new soundplane::Client(sOutput);
        sClient->setProperty("max_touches", 8);
        sClient->setZone(soundplane::Zone::presetChromatic());

        sClientRunning = true;
    }

    lua_pushboolean(L, sClientRunning);
    return 1;
}

static int spn_client_stop(lua_State *L) {
    if (sClientRunning) {
        sTimers->stop();

        delete sClient;
        sClient = nullptr;
        sClientRunning = false;

        soundplane::Console() << "spn: stopped client\n";

        lua_pushboolean(L, true);
        return 1;
    }

    lua_pushboolean(L, false);
    return 1;
}

static int spn_client_is_running(lua_State *L) {
    lua_pushboolean(L, sClientRunning);
    return 1;
}

static int spn_client_verbose(lua_State *L) { return 0; }

static int spn_client_set_property(lua_State *L) {
    if (!sClientRunning) {
        return luaL_error(L, "client not running");
    }

    const char *name = luaL_checkstring(L, 1);
    const float value = luaL_checknumber(L, 2);
    sClient->setProperty(name, value);
    lua_pushboolean(L, true);
    return 1;
}

static int spn_client_get_property(lua_State *L) {
    if (!sClientRunning) {
        return luaL_error(L, "client not running");
    }

    const char *name = luaL_checkstring(L, 1);
    const float value = sClient->getFloatProperty(name);
    lua_pushnumber(L, value);
    lua_pushboolean(L, true);
    return 2;
}

static int spn_client_zone_preset(lua_State *L) {
    if (!sClientRunning) {
        return luaL_error(L, "client not running");
    }

    bool was_set = false;
    uint8_t idx = luaL_checknumber(L, 1);
    switch (idx) {
    case 0:
        sClient->setZone(soundplane::Zone::presetChromatic());
        // sClient->setZone(soundplane::ZoneSpec::presetChromatic());
        was_set = true;
        break;
    case 1:
        sClient->setZones(soundplane::Zone::presetRowsInFourths());
        was_set = true;
        break;
    case 2:
        sClient->setZones(soundplane::Zone::presetRowsInOctaves());
        was_set = true;
        break;
    }

    lua_pushboolean(L, was_set);
    return 1;
}

static int spn_client_set_zones(lua_State *L) {
    std::vector<soundplane::Zone> zones;

    if (!lua_istable(L, -1)) {
        return luaL_error(L, "expected a table of zones");
    }

    if (!sClientRunning) {
        return luaL_error(L, "client not running");
    }

    lua_Integer num_zones = luaL_len(L, -1);
    for (int i = 1; i <= num_zones; i++) {
        lua_geti(L, -1, 1);
        spn_zone_t *z = spn_zone_check(L, -1);
        zones.push_back(soundplane::Zone::fromSpec(*(z->spec)));
        lua_pop(L, 1);
    }

    if (zones.size() > 0) {
        sClient->setZones(zones);
    } else {
        return luaL_error(L, "no zones specifications provided");
    }

    return 0;
}

static int spn_client_calibrate(lua_State *L) {
    if (!sClientRunning) {
        return luaL_error(L, "client not running");
    }
    sClient->beginCalibrate();
    return 0;
}
