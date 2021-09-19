#include "SPNTouchPool.h"

#include <boost/pool/object_pool.hpp>

static boost::object_pool<SPNTouch> sPool{128 /* initial segment */, 512 /* max segment size */};

SPNTouch *SPNTouchPool::construct(int index, float x, float y, float z, float note, int state) {
    return sPool.construct(SPNTouch(index, x, y, z, note, state));
}

void SPNTouchPool::destroy(SPNTouch *t) {
    sPool.destroy(t);
}

