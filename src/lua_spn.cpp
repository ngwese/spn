#include "lua_spn.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

// lua
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

extern "C" {
// matron
#include "event_types.h"
#include "events.h"
}

// soundplaneclient
#include "Client.h"
#include "Logging.h"
#include "Touch.h"
#include "Zone.h"

// this
#include "spn_touch.h"
#include "spn_zone.h"
// #include "SPNOutput.h"

//
// module definition
//

// clang-format off
static const luaL_Reg mod[] = {
    {"touch", spn_touch_open},
    // {"zone", spn_zone_open},
    {NULL, NULL}
};

static luaL_Reg func[] = {
    {NULL, NULL}
};
// clang-format on

LUA_SPN_API int luaopen_spn(lua_State *L) {
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
