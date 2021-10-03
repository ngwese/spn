
#include "SPNOutput.h"
#include "SPNControl.h"
#include "SPNControlPool.h"
#include "SPNFrame.h"
#include "SPNTouch.h"
#include "SPNTouchPool.h"

#include "spn_touch.h"

// lua
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

extern "C" {
// matron
#include "event_custom.h"
#include "event_types.h"
#include "events.h"
}

using namespace std::chrono;
using namespace soundplane;

//
// custom event glue
//

static inline void _push_module_func(lua_State *lvm, const char *module,
                                     const char *field, const char *func) {
    lua_getglobal(lvm, module);
    lua_getfield(lvm, -1, field);
    lua_remove(lvm, -2);
    lua_getfield(lvm, -1, func);
    lua_remove(lvm, -2);
}

static void spn_frame_begin_weave_op(lua_State *lvm, void *value,
                                     void *context) {
    uint32_t time = reinterpret_cast<uint32_t>(value);
    _push_module_func(lvm, "spn", "handlers", "frame_begin");
    lua_pushinteger(lvm, time); // 1s based index for lua
    lua_pcall(lvm, 1, 0,
              0); // one argument, zero results, default error message
}

static void spn_frame_end_weave_op(lua_State *lvm, void *value, void *context) {
    _push_module_func(lvm, "spn", "handlers", "frame_end");
    lua_pcall(lvm, 0, 0,
              0); // one argument, zero results, default error message
}

static void spn_touch_weave_op(lua_State *lvm, void *value, void *context) {
    SPNTouch *t = static_cast<SPNTouch *>(value);
    _push_module_func(lvm, "spn", "handlers", "touch");
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

static void spn_control_weave_op(lua_State *lvm, void *value, void *context) {
    SPNControl *c = static_cast<SPNControl *>(value);
    _push_module_func(lvm, "spn", "handlers", "control");
    lua_pushstring(lvm, c->name.getUTF8Ptr());
    lua_pushnumber(lvm, c->x);
    lua_pushnumber(lvm, c->y);
    lua_pushnumber(lvm, c->z);
    lua_pushinteger(lvm, c->n1);
    lua_pushinteger(lvm, c->n2);
    lua_pcall(lvm, 6, 0,
              0); // one argument, zero results, default error message
    SPNControlPool::destroy(c);
}

static void spn_null_free_op(void *value, void *context) {}

static struct event_custom_ops spn_touch_ops = {
    .type_name = "spn_touch",
    .weave = &spn_touch_weave_op,
    .free = &spn_null_free_op,
};

static struct event_custom_ops spn_control_ops = {
    .type_name = "spn_control",
    .weave = &spn_control_weave_op,
    .free = &spn_null_free_op,
};

static struct event_custom_ops spn_frame_begin_ops = {
    .type_name = "spn_frame_begin",
    .weave = &spn_frame_begin_weave_op,
    .free = &spn_null_free_op,
};

static struct event_custom_ops spn_frame_end_ops = {
    .type_name = "spn_frame_end",
    .weave = &spn_frame_end_weave_op,
    .free = &spn_null_free_op,
};

//
// output implementation
//

void SPNOutput::beginOutputFrame(time_point<system_clock> now) {
    auto now_ms = time_point_cast<milliseconds>(now);
    uint32_t value = now_ms.time_since_epoch().count();
    union event_data *ev = event_custom_new(
        &spn_frame_begin_ops, reinterpret_cast<void *>(value), NULL);
    event_post(ev);
}

void SPNOutput::processTouch(int i, int offset, const Touch &t) {
    if (touchIsActive(t)) {
        // grab a value from the pool an push into the event queue
        SPNTouch *value =
            SPNTouchPool::construct(i, t.x, t.y, t.z, t.note, t.state);
        union event_data *ev = event_custom_new(&spn_touch_ops, value, NULL);
        event_post(ev);
    }
}

void SPNOutput::processController(int zoneID, int offset,
                                  const ZoneMessage &m) {
    if (m.active) {
        SPNControl *value = SPNControlPool::construct(m.name, m.x, m.y, m.z,
                                                      m.number1, m.number2);
        union event_data *ev = event_custom_new(&spn_control_ops, value, NULL);
        event_post(ev);
    }
}

void SPNOutput::endOutputFrame() {
    union event_data *ev = event_custom_new(&spn_frame_end_ops, NULL, NULL);
    event_post(ev);
}
