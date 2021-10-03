#pragma once

#include "SPNControl.h"

class SPNControlPool {
  public:
    static SPNControl *construct(const ml::Symbol &name, float x, float y,
                                 float z, int n1, int n2 = 0);
    static void destroy(SPNControl *c);
};
