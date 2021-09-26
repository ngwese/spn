#include "SPNOutput.h"
#include "SPNTouch.h"
#include "SPNTouchPool.h"

#include "spn_touch.h"

// lua
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

extern "C" {
// matron
#include "event_types.h"
#include "event_custom.h"
#include "events.h"
}

using namespace std::chrono;
using namespace soundplane;

//
// custom event glue
//

static inline void _push_module_func(lua_State *lvm, const char *module, const char *func) {
    lua_getglobal(lvm, module);
    lua_getfield(lvm, -1, func);
    lua_remove(lvm, -2);
}

static void spn_weave_op(lua_State *lvm, void *value, void *context) {
    SPNTouch *t = static_cast<SPNTouch *>(value);
    _push_module_func(lvm, "spn", "touch");
    // spn_touch_new(lvm, t, true /* is_owned */);
    lua_pushinteger(lvm, t->index + 1); // 1s based index for lua
    lua_pushnumber(lvm, t->x);
    lua_pushnumber(lvm, t->y);
    lua_pushnumber(lvm, t->z);
    lua_pushnumber(lvm, t->note);
    lua_pushinteger(lvm, t->state);
    lua_pcall(lvm, 6, 0,
              0); // one argument, zero results, default error message
    SPNTouchPool::destroy(t);
}

static void spn_free_op(void *value, void *context) {
    // NOTE: nothing to do here since the spn_touch glue will free the SPNTouch
    // back to the pool on __gc

    // SPNTouch *t = static_cast<SPNTouch*>(value);
    // SPNTouchPool::destroy(t);
}

static struct event_custom_ops spn_ops = {
    .type_name = "spn",
    .weave = &spn_weave_op,
    .free = &spn_free_op,
};

//
// output implementation
//

void SPNOutput::beginOutputFrame(time_point<system_clock> now) {}

void SPNOutput::processTouch(int i, int offset, const Touch &t) {
    if (touchIsActive(t)) {
        // grab a value from the pool an push into the event queue
        SPNTouch *value =
            SPNTouchPool::construct(i, t.x, t.y, t.z, t.note, t.state);
        union event_data *ev = event_custom_new(&spn_ops, value, NULL);
        event_post(ev);
    }
}

void SPNOutput::processController(int zoneID, int offset,
                                  const ZoneMessage &m) {
    // TODO:
}

void SPNOutput::endOutputFrame() {}
