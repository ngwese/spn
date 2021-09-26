#include "SPNTouchPool.h"

#include <mutex>
#include <boost/pool/object_pool.hpp>

static std::mutex sPoolMutex;
static boost::object_pool<SPNTouch> sPool{128 /* initial segment */,
                                          512 /* max segment size */};


SPNTouch *SPNTouchPool::construct(int index, float x, float y, float z,
                                  float note, int state) {
    std::lock_guard<std::mutex> guard(sPoolMutex);
    return sPool.construct(SPNTouch(index, x, y, z, note, state));
}

void SPNTouchPool::destroy(SPNTouch *t) {
    std::lock_guard<std::mutex> guard(sPoolMutex);
    sPool.destroy(t);
}
