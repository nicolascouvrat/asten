#ifndef GUARD_SPY_INTERFACE_H
#define GUARD_SPY_INTERFACE_H

#include <array>

#include "io_interface.h"

// SpyInterface is a wrapper around another IOInterface.
//
// It will mimic its properties all the while sending usage statistics
class SpyInterface: public IOInterface {
  public:
    SpyInterface(InterfaceType targetType);
    bool shouldClose();
    bool shouldReset();
    void render();
    void colorPixel(int x, int y, int palette);
    std::array<ButtonSet, 2> getButtons();
  private:
    IOInterface *target;
};

#endif
