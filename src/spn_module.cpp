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
        lua_pushboolean(L, false);
        return 1;
    }

    const char *name = luaL_checkstring(L, 1);
    const float value = luaL_checknumber(L, 2);
    sClient->setProperty(name, value);
    lua_pushboolean(L, true);
    return 1;
}

static int spn_client_get_property(lua_State *L) {
    if (!sClientRunning) {
        lua_pushnumber(L, 0.0);
        lua_pushboolean(L, false);
        return 2;
    }

    const char *name = luaL_checkstring(L, 1);
    const float value = sClient->getFloatProperty(name);
    lua_pushnumber(L, value);
    lua_pushboolean(L, true);
    return 2;
}