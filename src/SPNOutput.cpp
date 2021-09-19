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
#include "events.h"
}

using namespace std::chrono;
using namespace soundplane;

//
// custom event glue
//

static void spn_weave_op(void *value, lua_State *lvm) {
    SPNTouch *t = static_cast<SPNTouch *>(value);
    lua_getglobal(lvm, "event_demo_handler");
    spn_touch_new(lvm, t, true /* is_owned */);
    lua_pcall(lvm, 1, 0,
              0); // one argument, zero results, default error message
}

static void spn_free_op(void *value) {
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
        union event_data *ev = event_custom_new(&spn_ops, value);
        event_post(ev);
    }
}

void SPNOutput::processController(int zoneID, int offset,
                                  const ZoneMessage &m) {
    // TODO:
}

void SPNOutput::endOutputFrame() {}
