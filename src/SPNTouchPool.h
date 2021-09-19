#pragma once

#include "SPNTouch.h"

class SPNTouchPool {
  public:
    static SPNTouch *construct(int index, float x, float y, float z, float note,
                               int state);
    static void destroy(SPNTouch *t);
};
