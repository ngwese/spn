#include "SPNControlPool.h"

#include <boost/pool/object_pool.hpp>
#include <mutex>

static std::mutex sPoolMutex;
static boost::object_pool<SPNControl> sPool{128 /* initial segment */,
                                            512 /* max segment size */};

SPNControl *SPNControlPool::construct(const ml::Symbol &name, float x, float y,
                                      float z, int n1, int n2) {
    std::lock_guard<std::mutex> guard(sPoolMutex);
    return sPool.construct(SPNControl(name, x, y, z, n1, n2));
}

void SPNControlPool::destroy(SPNControl *c) {
    std::lock_guard<std::mutex> guard(sPoolMutex);
    sPool.destroy(c);
}
