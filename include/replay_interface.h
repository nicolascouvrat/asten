#ifndef GUARD_REPLAY_INTERFACE_H
#define GUARD_REPLAY_INTERFACE_H

#include <array>
#include <fstream>

#include "io_interface.h"

// SpyInterface is a wrapper around another IOInterface.
//
// It will mimic its properties all the while sending usage statistics
class ReplayInterface: public IOInterface {
  public:
    ReplayInterface(InterfaceType targetType);
    bool shouldClose();
    bool shouldReset();
    void render();
    void colorPixel(int x, int y, int palette);
    std::array<ButtonSet, 2> getButtons();
  private:
    IOInterface *target;
    std::ifstream in;
    bool done;
};

#endif
