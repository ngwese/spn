#pragma once

#include <chrono>

#include "Output.h"

class SPNOutput : public soundplane::Output {
  public:
    virtual void beginOutputFrame(
        std::chrono::time_point<std::chrono::system_clock> now) override;
    virtual void processTouch(int i, int offset,
                              const soundplane::Touch &t) override;
    virtual void processController(int zoneID, int offset,
                                   const soundplane::ZoneMessage &m) override;
    virtual void endOutputFrame() override;
};
